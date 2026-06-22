#include "installedappsmodel.h"

#include "../tools/packageutils.h"
#include "../tools/settings.h"

#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QChar>
#include <algorithm>

namespace MeeShop {

InstalledAppsModel::InstalledAppsModel(QObject *parent)
    : QAbstractListModel(parent) {
    QHash<int, QByteArray> roles;
    roles[NameRole]        = "name";
    roles[DeveloperRole]   = "developer";
    roles[IconSourceRole]  = "iconSource";
    roles[SizeTextRole]    = "sizeText";
    roles[PackageNameRole] = "packageName";
    roles[SectionRole]     = "section";
    setRoleNames(roles);
}

int InstalledAppsModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_entries.size();
}

QVariant InstalledAppsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
        return QVariant();
    const Entry &e = m_entries.at(index.row());
    switch (role) {
    case NameRole:        return e.name;
    case DeveloperRole:   return e.developer;
    case IconSourceRole:  return e.iconSource;
    case SizeTextRole:    return e.sizeText;
    case PackageNameRole: return e.packageName;
    case SectionRole:     return e.section;
    }
    return QVariant();
}

// ----------------------------------------------------------------- refresh

void InstalledAppsModel::refresh() {
    const QStringList pkgs = Settings::getInstance().getMeeshopPackages();
    const QMultiHash<QString, QVariantMap> dpkg =
            PackageUtils::parsePkgDatabase(QString::fromLatin1("/var/lib/dpkg/status"));

    QList<Entry> entries;
    for (int i = 0; i < pkgs.size(); ++i) {
        const QString pkg = pkgs.at(i);
        if (pkg.isEmpty())
            continue;

        // Показываем только реально установленные пакеты (запись могла устареть,
        // если пакет удалили в обход MeeShop).
        if (!dpkg.contains(pkg))
            continue;
        const QVariantMap info = dpkg.values(pkg).at(0);
        if (!info.value(QString::fromLatin1("Status")).toString()
                 .contains(QString::fromLatin1("install ok installed")))
            continue;

        Entry e;
        e.packageName = pkg;

        const qint64 sizeKb = info.value(QString::fromLatin1("Installed-Size")).toLongLong();
        e.sizeText = formatSize(sizeKb * 1024);
        e.developer = cleanDeveloper(info.value(QString::fromLatin1("Maintainer")).toString());

        // Стабильные данные из desktop-файла берём из кэша (или вычисляем и кэшируем),
        // чтобы повторные refresh() не перезапускали `dpkg -L` и разбор файла.
        QStringList di = m_desktopCache.value(pkg);
        if (di.isEmpty()) {
            const QString df = desktopFileForPackage(pkg);
            QString dName, dIcon;
            if (!df.isEmpty())
                parseDesktopFile(df, dName, dIcon);
            di << df << dName << resolveIcon(dIcon); // всегда 3 элемента (кэшируем и пустое)
            m_desktopCache.insert(pkg, di);
        }
        e.desktopFile = di.at(0);
        e.name = di.at(1).isEmpty() ? pkg : di.at(1);
        e.iconSource = di.at(2);
        // Если локальная иконка из .desktop не нашлась — берём сохранённый при
        // установке URL иконки openrepos (та же иконка, что в магазине).
        if (e.iconSource.isEmpty())
            e.iconSource = Settings::getInstance().iconForPackage(pkg);
        e.section = sectionForName(e.name);

        entries.append(e);
    }

    rebuild(entries);
}

void InstalledAppsModel::rebuild(QList<Entry> entries) {
    std::sort(entries.begin(), entries.end(), &InstalledAppsModel::lessThan);
    beginResetModel();
    m_entries = entries;
    endResetModel();
    emit countChanged();
}

// ----------------------------------------------------------------- actions

void InstalledAppsModel::launch(int row) {
    if (row < 0 || row >= m_entries.size())
        return;
    const QString desktop = m_entries.at(row).desktopFile;
    if (desktop.isEmpty())
        return;
    QProcess::startDetached(QString::fromLatin1("/usr/bin/xdg-open"),
                            QStringList() << desktop);
}

bool InstalledAppsModel::isRunnable(int row) const {
    if (row < 0 || row >= m_entries.size())
        return false;
    return !m_entries.at(row).desktopFile.isEmpty();
}

// -------------------------------------------------------------- navigation

QStringList InstalledAppsModel::sections() const {
    QStringList out;
    for (int i = 0; i < m_entries.size(); ++i) {
        const QString &s = m_entries.at(i).section;
        if (out.isEmpty() || out.last() != s) // записи отсортированы => секции подряд
            out.append(s);
    }
    return out;
}

int InstalledAppsModel::firstIndexOfSection(const QString &section) const {
    for (int i = 0; i < m_entries.size(); ++i)
        if (m_entries.at(i).section == section)
            return i;
    return -1;
}

// ------------------------------------------------------------- test helpers

void InstalledAppsModel::addPlaceholders() {
    struct P { const char *name; const char *dev; const char *icon; qint64 kb; };
    static const P samples[] = {
        { "Angry Birds",   "Rovio",        "icon-m-content-ovi-store-inverse", 24576 },
        { "Battery Buddy", "Heliopolis",   "icon-m-toolbar-settings",           1536 },
        { "Cargo Dock",    "Tuomas Kulve", "icon-m-toolbar-directory",          8192 },
        { "Cool Reader",   "Vadim Lopatin","icon-m-toolbar-list",              15300 },
        { "Drum Pads",     "Alenz",        "icon-m-toolbar-mediacontrol-play",  3072 },
        { "F-Spot",        "Novell",       "icon-m-toolbar-camera",            51200 },
        { "Mappero",       "Max Usachev",  "icon-m-toolbar-region",           102400 },
        { "Quasar MX",     "CuteFishApps", "icon-m-toolbar-mediacontrol-play", 18000 },
        { "Tweetian",      "Dickson Leong","icon-m-toolbar-favorite-mark",      9800 },
        { "Wikipedia",     "Wikimedia",    "icon-m-toolbar-list",              30720 },
        { "Yappari",       "Eduardo",      "icon-m-toolbar-send-chat",         22000 },
        { "1Mobile",       "OneStore",     "icon-m-toolbar-grid",               4096 }
    };
    QList<Entry> all = m_entries;
    for (std::size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        Entry e;
        e.name = QString::fromLatin1(samples[i].name);
        e.developer = QString::fromLatin1(samples[i].dev);
        e.iconSource = QString::fromLatin1("image://theme/") + QString::fromLatin1(samples[i].icon);
        e.sizeText = formatSize(samples[i].kb * 1024);
        e.packageName = QString::fromLatin1("test-") + e.name.toLower().replace(' ', '-');
        e.desktopFile = QString(); // плейсхолдеры не запускаются
        e.section = sectionForName(e.name);
        all.append(e);
    }
    rebuild(all);
}

void InstalledAppsModel::clear() {
    rebuild(QList<Entry>());
}

// ------------------------------------------------------------------ helpers

QString InstalledAppsModel::formatSize(qint64 bytes) {
    if (bytes <= 0)
        return QString();
    const double kb = bytes / 1024.0;
    const double mb = kb / 1024.0;
    const double gb = mb / 1024.0;
    if (gb >= 1.0)
        return QString::number(gb, 'f', 2) + QString::fromLatin1(" GB");
    if (mb >= 1.0)
        return QString::number(mb, 'f', 2) + QString::fromLatin1(" MB");
    return QString::number(kb, 'f', 2) + QString::fromLatin1(" KB");
}

QString InstalledAppsModel::sectionForName(const QString &name) {
    if (name.isEmpty())
        return QString::fromLatin1("#");
    const QChar c = name.at(0);
    if (c.isLetter())
        return QString(c).toUpper();
    return QString::fromLatin1("#");
}

bool InstalledAppsModel::lessThan(const Entry &a, const Entry &b) {
    if (a.section != b.section) {
        // «#» (цифры/символы) всегда в самом конце; буквенные секции — по алфавиту.
        if (a.section == QString::fromLatin1("#")) return false;
        if (b.section == QString::fromLatin1("#")) return true;
        return a.section < b.section;
    }
    return QString::compare(a.name, b.name, Qt::CaseInsensitive) < 0;
}

QString InstalledAppsModel::desktopFileForPackage(const QString &package) {
    QProcess dpkg;
    dpkg.start(QString::fromLatin1("/bin/dpkg"), QStringList()
               << QString::fromLatin1("-L") << package);
    if (!dpkg.waitForStarted(3000)) {
        dpkg.kill();
        dpkg.waitForFinished(1000);
        return QString();
    }
    if (!dpkg.waitForFinished(4000)) {
        // dpkg завис (напр., БД заблокирована только что отработавшим apt) — не даём
        // ему заморозить UI-поток: убиваем и работаем с тем, что успели прочитать.
        dpkg.kill();
        dpkg.waitForFinished(1000);
    }

    const QByteArray out = dpkg.readAllStandardOutput();
    const QList<QByteArray> lines = out.split('\n');
    for (int i = 0; i < lines.size(); ++i) {
        const QString line = QString::fromUtf8(lines.at(i)).trimmed();
        if (line.startsWith(QString::fromLatin1("/usr/share/applications/"))
                && line.endsWith(QString::fromLatin1(".desktop")))
            return line;
    }
    return QString();
}

void InstalledAppsModel::parseDesktopFile(const QString &path, QString &name, QString &icon) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&f);
    bool inEntry = false;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.startsWith('[')) {                       // смена группы
            inEntry = (line == QString::fromLatin1("[Desktop Entry]"));
            continue;
        }
        if (!inEntry)
            continue;
        // Берём нелокализованные ключи (Name=, не Name[ru]=).
        if (name.isEmpty() && line.startsWith(QString::fromLatin1("Name=")))
            name = line.mid(5).trimmed();
        else if (icon.isEmpty() && line.startsWith(QString::fromLatin1("Icon=")))
            icon = line.mid(5).trimmed();
        if (!name.isEmpty() && !icon.isEmpty())
            break;
    }
    f.close();
}

QString InstalledAppsModel::resolveIcon(const QString &iconField) {
    if (iconField.isEmpty())
        return QString();

    // Абсолютный путь — используем как есть, если файл существует.
    if (iconField.startsWith('/'))
        return QFile::exists(iconField) ? QString::fromLatin1("file://") + iconField : QString();

    // Голое имя темы — ищем в типичных для Harmattan местах.
    QStringList names;
    names << iconField;
    if (!iconField.contains('.'))
        names << iconField + QString::fromLatin1(".png")
              << iconField + QString::fromLatin1(".svg");

    QStringList dirs;
    dirs << QString::fromLatin1("/usr/share/icons/hicolor/80x80/apps/")
         << QString::fromLatin1("/usr/share/icons/hicolor/64x64/apps/")
         << QString::fromLatin1("/usr/share/icons/hicolor/48x48/apps/")
         << QString::fromLatin1("/usr/share/icons/hicolor/scalable/apps/")
         << QString::fromLatin1("/usr/share/pixmaps/")
         << QString::fromLatin1("/usr/share/themes/blanco/meegotouch/icons/")
         << QString::fromLatin1("/usr/share/icons/");

    for (int d = 0; d < dirs.size(); ++d)
        for (int n = 0; n < names.size(); ++n) {
            const QString full = dirs.at(d) + names.at(n);
            if (QFile::exists(full))
                return QString::fromLatin1("file://") + full;
        }
    return QString();
}

QString InstalledAppsModel::cleanDeveloper(const QString &maintainer) {
    if (maintainer.isEmpty())
        return QString();
    const int lt = maintainer.indexOf('<');     // «Имя <email>» -> «Имя»
    if (lt > 0)
        return maintainer.left(lt).trimmed();
    return maintainer.trimmed();
}

}

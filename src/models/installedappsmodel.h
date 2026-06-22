#ifndef INSTALLEDAPPSMODEL_H
#define INSTALLEDAPPSMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QByteArray>
#include <QMap>

namespace MeeShop {

// Список приложений, установленных через MeeShop (источник истины — список пакетов
// в Settings). Каждый пакет обогащается данными с устройства: размер и
// сопровождающий из dpkg-статуса, имя/иконка/.desktop — из его desktop-файла.
// Записи отсортированы по имени; роль `section` (заглавная первая буква, «#» для
// прочего) используется для секций ListView и быстрой навигации по алфавиту.
class InstalledAppsModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        DeveloperRole,
        IconSourceRole,
        SizeTextRole,
        PackageNameRole,
        SectionRole
    };

    struct Entry {
        QString name;
        QString developer;
        QString iconSource;   // готовый source для Image (file://… либо пусто)
        QString sizeText;     // отформатированный размер (напр. «12.30 MB»)
        QString packageName;
        QString desktopFile;  // путь к .desktop (для запуска), может быть пуст
        QString section;      // «A»…«Z» либо «#»
    };

    explicit InstalledAppsModel(QObject *parent = 0);

    int count() const { return m_entries.size(); }

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // Перечитать список из Settings и обогатить данными с устройства.
    Q_INVOKABLE void refresh();

    // Запустить установленное приложение (xdg-open его .desktop). Ничего не делает,
    // если у записи нет desktop-файла.
    Q_INVOKABLE void launch(int row);
    Q_INVOKABLE bool isRunnable(int row) const;

    // Для быстрой навигации по алфавиту (драг по scroll decorator).
    Q_INVOKABLE QStringList sections() const;
    Q_INVOKABLE int firstIndexOfSection(const QString &section) const;

    // --- Тестовые помощники (для проверки UI без реальных установок) ---
    Q_INVOKABLE void addPlaceholders();
    Q_INVOKABLE void clear();

    // Отформатировать размер в КБ/МБ/ГБ (две цифры после запятой). База 1024.
    static QString formatSize(qint64 bytes);

signals:
    void countChanged();

private:
    void rebuild(QList<Entry> entries); // отсортировать и заменить содержимое
    static QString sectionForName(const QString &name);
    static bool lessThan(const Entry &a, const Entry &b);

    // Обогащение одного пакета (dpkg + desktop-файл).
    static QString desktopFileForPackage(const QString &package);
    static void parseDesktopFile(const QString &path, QString &name, QString &icon);
    static QString resolveIcon(const QString &iconField);
    static QString cleanDeveloper(const QString &maintainer);

    QList<Entry> m_entries;

    // Кэш дорогих стабильных данных из desktop-файла (package -> [desktopFile, name,
    // iconSource]). desktopFile/имя/иконка установленного пакета не меняются, поэтому
    // повторные refresh() не перезапускают `dpkg -L` и разбор файла (важно: refresh
    // зовётся по завершении задач очереди). Размер/разработчик берём из dpkg каждый раз.
    QHash<QString, QStringList> m_desktopCache;
};

}

#endif // INSTALLEDAPPSMODEL_H

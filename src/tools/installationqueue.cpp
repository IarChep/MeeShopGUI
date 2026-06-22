#include "installationqueue.h"
#include "settings.h"

#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QHash>
#include <QDebug>

namespace MeeShop {

InstallationQueue::InstallationQueue(QObject *parent)
    : QObject(parent),
      apt(QString::fromLatin1("/usr/bin/aegis-apt-get")),
      m_model(new InstallationQueueModel(this)),
      m_activeAppId(-1),
      m_stepIndex(0) {
    connect(&apt, SIGNAL(actionChanged(QString,bool)), this, SLOT(onAptAction(QString,bool)));
    connect(&apt, SIGNAL(progress(QString,int)),  this, SLOT(onAptProgress(QString,int)));
    connect(&apt, SIGNAL(logLine(int,QString)),   this, SLOT(onAptLogLine(int,QString)));
    connect(&apt, SIGNAL(finished(int,QString)),  this, SLOT(onAptFinished(int,QString)));
    connect(m_model, SIGNAL(activeCountChanged()), this, SIGNAL(activeCountChanged()));
}

// ---------------------------------------------------------------- queue API

void InstallationQueue::enqueue(QVariantMap app, int operation) {
    const int appId = app.value("appid").toInt();
    if (appId == 0 || contains(appId))
        return;

    const QString name = app.value("title").toString().trimmed();
    const QString developer = app.value("user").toMap().value("name").toString();
    const QString iconUrl = app.value("icon").toMap().value("url").toString();

    QString packageName = app.value("packages").toMap().value("harmattan").toMap().value("name").toString();
    if (packageName.isEmpty())
        packageName = app.value("package").toMap().value("name").toString();

    // Не дублируем удаление одного пакета (могло уже прийти со страницы установленных).
    if (operation == Remove && activeRemovalRow(packageName) >= 0)
        return;

    m_model->addEntry(appId, name, developer, iconUrl, packageName, operation);
    processNext();
}

void InstallationQueue::enqueueRemovalByPackage(const QString &name, const QString &developer,
                                                const QString &iconUrl, const QString &packageName) {
    if (packageName.isEmpty())
        return;
    // Не дублируем удаление одного и того же пакета.
    if (activeRemovalRow(packageName) >= 0)
        return;
    // Синтетический appId: у установленного приложения нет id с openrepos. Берём
    // уникальный счётчик в высоком диапазоне (>= 1e9) — не пересекается с небольшими
    // id openrepos и не сталкивается с другими (в отличие от хеша). Дедуп — по имени
    // пакета выше, не по id. QueueDelegate не открывает AppPage для таких id.
    static int counter = 0;
    const int appId = 1000000000 + (++counter);
    m_model->addEntry(appId, name.isEmpty() ? packageName : name,
                      developer, iconUrl, packageName, Remove);
    processNext();
}

int InstallationQueue::activeRemovalRow(const QString &packageName) const {
    for (int i = 0; i < m_model->count(); ++i) {
        InstallationQueueModel::Entry *e = m_model->at(i);
        if (e && e->operation == Remove && e->packageName == packageName
                && (e->status == Queued || e->status == Running))
            return i;
    }
    return -1;
}

bool InstallationQueue::contains(int appId) const {
    return m_model->indexOfAppId(appId) >= 0;
}

int InstallationQueue::taskStatus(int appId) const {
    InstallationQueueModel::Entry *e = m_model->byAppId(appId);
    return e ? e->status : -1;
}

void InstallationQueue::cancel(int appId) {
    const int row = m_model->indexOfAppId(appId);
    if (row < 0)
        return;
    InstallationQueueModel::Entry *e = m_model->at(row);
    if (e->status == Running) // активную операцию не прерываем
        return;
    m_model->removeRow(row);
}

void InstallationQueue::move(int from, int to) {
    // Двигать можно только ожидающие задачи: за время перетаскивания приложение
    // могло начать выполняться (processNext), тогда перемещение запрещено.
    InstallationQueueModel::Entry *e = m_model->at(from);
    if (!e || e->status != Queued)
        return;
    // И не выше границы — ожидающее нельзя поставить над выполняемыми/завершёнными.
    const int minTo = firstQueuedIndex();
    if (to < minTo)
        to = minTo;
    m_model->moveRow(from, to);
}

int InstallationQueue::firstQueuedIndex() const {
    for (int i = 0; i < m_model->count(); ++i)
        if (m_model->at(i)->status == Queued)
            return i;
    return m_model->count();
}

QObject *InstallationQueue::logForApp(int appId) const {
    InstallationQueueModel::Entry *e = m_model->byAppId(appId);
    return e ? e->log : 0;
}

// ---------------------------------------------------------- тестовые помощники

void InstallationQueue::addPlaceholder(int status) {
    static int counter = 0;
    ++counter;
    // Отрицательный appId — чтобы не пересекаться с реальными.
    const int appId = -1000 - counter;
    const QString name = QString::fromLatin1("Test App %1").arg(counter);
    const QString pkg  = QString::fromLatin1("test-pkg-%1").arg(counter);
    const int row = m_model->addEntry(appId, name, QString::fromLatin1("Placeholder Dev"),
                                      QString(), pkg, Install);

    InstallationQueueModel::Entry *e = m_model->at(row);
    if (e && e->log && status != Queued) {
        e->log->append(AptLogModel::Info, QString::fromLatin1("Reading package lists..."));
        e->log->append(AptLogModel::Info, QString::fromLatin1("Building dependency tree..."));
        e->log->append(AptLogModel::Info,
                       QString::fromLatin1("Get:1 http://harmattan.openrepos.net personal %1").arg(name));
        if (status == Failed) {
            e->log->append(AptLogModel::Warning,
                           QString::fromLatin1("W: GPG error: signature could not be verified"));
            e->log->append(AptLogModel::Error, QString::fromLatin1("E: Unable to fetch %1").arg(pkg));
        }
    }
    if (status == Running) {
        m_model->setAction(row, QString::fromLatin1("Downloading…"), false);
        m_model->setProgress(row, 42);
    }
    if (status != Queued)
        m_model->setStatus(row, status);
}

void InstallationQueue::clearPlaceholders() {
    // Удаляем ТОЛЬКО плейсхолдеры (appId < 0) — реальные задачи (в т.ч.
    // выполняемую) не трогаем.
    for (int i = m_model->count() - 1; i >= 0; --i) {
        InstallationQueueModel::Entry *e = m_model->at(i);
        if (e && e->appId < 0)
            m_model->removeRow(i);
    }
}

// ---------------------------------------------------------------- processing

void InstallationQueue::processNext() {
    if (apt.isRunning() || m_activeAppId != -1)
        return;

    int row = -1;
    for (int i = 0; i < m_model->count(); ++i) {
        InstallationQueueModel::Entry *e = m_model->at(i);
        // Плейсхолдеры тестовой страницы (appId < 0) реальным apt не запускаем.
        if (e->status == Queued && e->appId >= 0) {
            row = i;
            break;
        }
    }
    if (row < 0)
        return;

    InstallationQueueModel::Entry *e = m_model->at(row);
    m_activeAppId = e->appId;
    m_model->setStatus(row, Running);
    emit taskStatusChanged(e->appId, Running);

    m_steps.clear();
    m_stepIndex = 0;
    switch (e->operation) {
    case Install:
        if (!isRepositoryEnabled(e->developer)) {
            enableRepository(e->developer);     // создаём .list (синхронно)
            m_steps << StepUpdate << StepInstall;
        } else {
            m_steps << StepInstall;
        }
        break;
    case Update:
        m_steps << StepInstall;                 // apt install обновляет до последней
        break;
    case Remove:
        m_steps << StepRemove;
        break;
    case RepoUpdate:
        m_steps << StepUpdate;
        break;
    default:
        m_steps << StepInstall;
        break;
    }
    runStep();
}

void InstallationQueue::runStep() {
    InstallationQueueModel::Entry *e = m_model->byAppId(m_activeAppId);
    if (!e || m_stepIndex < 0 || m_stepIndex >= m_steps.size())
        return;
    switch (m_steps.at(m_stepIndex)) {
    case StepUpdate:  apt.update();             break;
    case StepInstall: apt.install(e->packageName); break;
    case StepRemove:  apt.remove(e->packageName);  break;
    }
}

void InstallationQueue::onAptAction(const QString &action, bool determinate) {
    const int row = m_model->indexOfAppId(m_activeAppId);
    if (row < 0)
        return;
    // Определённость теперь приходит явно из парсера (а не «нюхом» по подстроке
    // "Downloading"). Третий аргумент модели — indeterminate, поэтому инвертируем.
    m_model->setAction(row, action, !determinate);
    emit taskActionChanged(m_activeAppId, action, determinate);
}

void InstallationQueue::onAptProgress(const QString &action, int percent) {
    Q_UNUSED(action);
    const int row = m_model->indexOfAppId(m_activeAppId);
    if (row < 0)
        return;
    m_model->setProgress(row, percent);
    InstallationQueueModel::Entry *e = m_model->at(row);
    emit taskProgressChanged(m_activeAppId, percent, e ? e->indeterminate : true);
}

void InstallationQueue::onAptLogLine(int level, const QString &message) {
    InstallationQueueModel::Entry *e = m_model->byAppId(m_activeAppId);
    if (e && e->log)
        e->log->append(static_cast<AptLogModel::Level>(level), message);
}

void InstallationQueue::onAptFinished(int code, const QString &output) {
    Q_UNUSED(output);
    const int appId = m_activeAppId;
    const int row = m_model->indexOfAppId(appId);
    if (row < 0) {
        m_activeAppId = -1;
        m_steps.clear();
        processNext();
        return;
    }
    InstallationQueueModel::Entry *e = m_model->at(row);

    if (code != 0) {
        m_model->setStatus(row, Failed);
        emit taskStatusChanged(appId, Failed);
        emit taskFinished(appId, code);
        m_activeAppId = -1;
        m_steps.clear();
        processNext();
        return;
    }

    // Шаг успешно завершён.
    if (m_stepIndex >= 0 && m_stepIndex < m_steps.size() && m_steps.at(m_stepIndex) == StepUpdate)
        m_repoPackagesCache.clear(); // apt update перезаписал файлы Packages

    m_stepIndex++;
    if (m_stepIndex < m_steps.size()) {
        runStep();
        return;
    }

    // Все шаги задачи выполнены.
    if (e->operation != RepoUpdate)
        cacheInstalledPackages();

    // Ведём список приложений, установленных через MeeShop, для страницы установленных.
    // Install/Update — добавляем (addMeeshopPackage идемпотентна; обновлённое приложение
    // тоже «управляется» MeeShop). Remove — снимаем из списка ТОЛЬКО если пакет реально
    // больше не установлен: apt может вернуть 0, фактически не удалив пакет (essential/
    // held/нюансы --force-yes), иначе приложение «осиротеет» — dpkg считает установленным,
    // а из списка оно пропало (и его уже не удалить/запустить через MeeShop).
    if (e->operation == Install || e->operation == Update) {
        // Сохраняем и URL иконки (с openrepos) — для страницы установленных.
        Settings::getInstance().addMeeshopApp(e->packageName, e->iconUrl);
    } else if (e->operation == Remove) {
        const bool stillInstalled = installedPackages.contains(e->packageName)
                && installedPackages.values(e->packageName).at(0)
                       .value("Status").toString().contains("install ok installed");
        if (!stillInstalled)
            Settings::getInstance().removeMeeshopPackage(e->packageName);
    }

    m_model->setStatus(row, Done);
    emit taskStatusChanged(appId, Done);
    emit taskFinished(appId, 0);
    m_activeAppId = -1;
    m_steps.clear();
    processNext();
}

// ---------------------------------------------------- synchronous queries

const QMultiHash<QString, QVariantMap> &InstallationQueue::repoPackages(const QString &name) {
    QHash<QString, QMultiHash<QString, QVariantMap> >::iterator it = m_repoPackagesCache.find(name);
    if (it == m_repoPackagesCache.end())
        it = m_repoPackagesCache.insert(name, PackageUtils::parsePkgDatabase(PackageUtils::getRepoPackagesPath(name)));
    return it.value();
}

int InstallationQueue::isInstalled(QString package, QString name) {
    if (installedPackages.contains(package)) {
        QVariantMap pkg = installedPackages.values(package).at(0);
        if (pkg["Status"] == "install ok installed") {
            QString maxRepoVersion = PackageUtils::findMaxVersion(package, repoPackages(name));
            QString installedVersion = pkg["Version"].toString();
            if ((installedVersion != maxRepoVersion) &&
                PackageUtils::compareVersions(installedVersion.toStdString(), maxRepoVersion.toStdString()) < 0)
                return Updatable;
            return Installed;
        }
        return NotInstalled;
    }
    return NotInstalled;
}

bool InstallationQueue::isRepositoryEnabled(QString name) {
    return enabledRepositories.contains(name) && enabledRepositories[name].toBool();
}

void InstallationQueue::cacheInstalledPackages() {
    installedPackages = PackageUtils::parsePkgDatabase("/var/lib/dpkg/status");
}

void InstallationQueue::cacheEnabledRepositories() {
    enabledRepositories.clear();
    m_repoPackagesCache.clear();

    QDir dir("/etc/apt/sources.list.d");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("meeshop-*.list*"));
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QString reponame = list.at(i).baseName();
        reponame.replace("meeshop-", "");
        enabledRepositories[reponame] = (list.at(i).completeSuffix() == "list");
    }
}

void InstallationQueue::enableRepository(QString name) {
    QFile repo(PackageUtils::getRepoPath(name));
    if (!repo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write repository file:" << repo.fileName();
        return;
    }
    QString repositoryString = QString("deb http://harmattan.openrepos.net/%1 personal main").arg(name);
    repo.write(repositoryString.toLocal8Bit());
    repo.close();
    cacheEnabledRepositories();
}

void InstallationQueue::disableRepository(QString name) {
    QFile::remove(PackageUtils::getRepoPath(name));
    cacheEnabledRepositories();
}

}

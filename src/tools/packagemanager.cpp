#include "packagemanager.h"


QString MeeShop::PackageManager::isInstalled(QString package, QString name) {
    qDebug() << "Checking package" << package;
    if (installedPackages.contains(package)) {
        QVariantMap pkg = installedPackages.values(package).at(0);
        if (pkg["Status"] == "install ok installed") {
            qDebug() << "Package" << package << "is installed";
            QString maxRepoVersion = PackageUtils::findMaxVersion(package, PackageUtils::getRepoPackagesPath(name));
            qDebug() << "Max version in the repo" << maxRepoVersion << ", current installde vesrion" << pkg["Version"].toString();
            if (PackageUtils::compareVersions(pkg["Version"].toString().toStdString(), maxRepoVersion.toStdString()) < 0) {
                qDebug() << "updateable";
                return "Updatable";
            } else {
                qDebug() << "final version";
                return "Installed";
            }
        } else {
            return "NotInstalled";
        }
    }
    qDebug() << "Package" << package << "is not installed";
    return "NotInstalled";
}

void MeeShop::PackageManager::updateRepositories() {
    qDebug() << "Updating repos";
    connect(&apt, SIGNAL(exited(int, const QString&)), this, SIGNAL(updateFinished(int,QString)));
    apt.updateRepos();
}

void MeeShop::PackageManager::installPackage(QString package) {
    qDebug() << "installing package " << package;
    connect(&apt, SIGNAL(exited(int, const QString&)), this, SIGNAL(installationFinished(int,QString)));
    apt.installPackage(package.toStdString());
}

void MeeShop::PackageManager::cacheInstalledPackages() {
    installedPackages = PackageUtils::parsePkgDatabase("/var/lib/dpkg/status");
    for (QMultiHash<QString, QVariantMap>::const_iterator it = installedPackages.constBegin(); it != installedPackages.constEnd(); ++it) {
        QVariantMap data = it.value();
        if (data["Status"].toString().contains("installed")) {
            qDebug().nospace()
                    << "Пакет: " << it.key() << "\n"
                    << "  Версия: " << data["Version"].toString() << "\n"
                    << "  Статус: " << data["Status"].toString() << "\n"
                    << "  Архитектура: " << data["Architecture"].toString() << "\n"
                    << "  Описание: " << data["Description"].toString();
        }
    }
}
bool MeeShop::PackageManager::isRepositoryEnabled(QString name) {
    return (enabledRepositories.contains(name) && enabledRepositories[name].toBool());
}

void MeeShop::PackageManager::cacheEnabledRepositories() {
    enabledRepositories.clear();

    QDir dir("/etc/apt/sources.list.d");
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("meeshop-*.list*"));
    QFileInfoList list = dir.entryInfoList();
    for (const auto& info : list) {
        QString reponame = info.baseName();
        reponame.replace("meeshop-","");
        enabledRepositories[reponame] = (info.completeSuffix() == "list");
    }
}
void MeeShop::PackageManager::enableRepository(QString name) {
    QFile repo(PackageUtils::getRepoPath(name));

    QString repositoryString = QString("deb http://harmattan.openrepos.net/%1 personal main").arg(name);
    repo.open(QIODevice::WriteOnly | QIODevice::Text);
    repo.write(repositoryString.toLocal8Bit());
    repo.close();
    cacheEnabledRepositories();
}
void MeeShop::PackageManager::disableRepository(QString name) {
    QFile::remove(PackageUtils::getRepoPath(name));
    cacheEnabledRepositories();
}


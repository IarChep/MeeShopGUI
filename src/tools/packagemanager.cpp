#include "packagemanager.h"


int MeeShop::PackageManager::isInstalled(QString package, QString name) {
    if (installedPackages.contains(package)) {
        QVariantMap pkg = installedPackages.values(package).at(0);
        if (pkg["Status"] == "install ok installed") {
            QString maxRepoVersion = PackageUtils::findMaxVersion(package, PackageUtils::getRepoPackagesPath(name));
            QString installedVersion = pkg["Version"].toString();
            if ( (installedVersion != maxRepoVersion) && PackageUtils::compareVersions(pkg["Version"].toString().toStdString(), maxRepoVersion.toStdString()) < 0) {
                return 2;
            } else {
                return 1;
            }
        } else {
            return 0;
        }
    }
    return 0;
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


#include "PackageManager.h"


MeeShop::PackageManager::PackageStatus MeeShop::PackageManager::isInstalled(QString package) {
    qDebug() << "Checking package" << package;
    if (installedPackages.contains(package)) {
        QVariantMap pkg = installedPackages.value(package).toMap();
        if (installedPackages[package].toMap()["Status"] == "install ok installed") {
            return MeeShop::PackageManager::PackageStatus::Installed;
        }
    }
    return MeeShop::PackageManager::PackageStatus::NotInstalled;
}

void MeeShop::PackageManager::updateRepositories() {
    apt.updateRepos();
}

void MeeShop::PackageManager::installPackage(QString package) {
    qDebug() << "installing package " << package;
    apt.installPackage(package.toStdString());
}

void MeeShop::PackageManager::cacheInstalledPackages() {
    installedPackages.clear();
    QFile file("/var/lib/dpkg/status");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не могу открыть файл статуса. Запустите с sudo?";
    }

    QTextStream in(&file);
    QVariantMap currentPackage;
    QString currentPackageName;
    bool inPackageBlock = false;

    // Регулярные выражения Qt4
    QRegExp packageRx("^Package:\\s*(\\S+)");
    QRegExp fieldRx("^([\\w-]+):\\s*(.*)");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // Начало нового пакета
        if (packageRx.indexIn(line) != -1) {
            if (inPackageBlock && !currentPackageName.isEmpty()) {
                installedPackages.insert(currentPackageName, currentPackage);
            }

            currentPackageName = packageRx.cap(1);
            currentPackage.clear();
            inPackageBlock = true;
        }
        // Обработка полей
        else if (inPackageBlock && !line.isEmpty()) {
            if (fieldRx.indexIn(line) != -1) {
                QString fieldName = fieldRx.cap(1);
                QString fieldValue = fieldRx.cap(2);
                currentPackage[fieldName] = fieldValue.trimmed();
            }
        }
        // Конец блока пакета
        else if (inPackageBlock && line.isEmpty()) {
            installedPackages.insert(currentPackageName, currentPackage);
            currentPackageName.clear();
            currentPackage.clear();
            inPackageBlock = false;
        }
    }

    // Добавляем последний пакет, если файл закончился без пустой строки
    if (inPackageBlock && !currentPackageName.isEmpty()) {
        installedPackages.insert(currentPackageName, currentPackage);
    }

    // Выводим только установленные пакеты
    QMapIterator<QString, QVariant> it(installedPackages);
    while (it.hasNext()) {
        it.next();
        QVariantMap data = it.value().toMap();

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
    QFile repo(QString("/etc/apt/sources.list.d/meeshop-%1.list").arg(name));
    QString repositoryString = QString("deb http://harmattan.openrepos.net/%1 personal main").arg(name);
    repo.open(QIODevice::WriteOnly | QIODevice::Text);
    repo.write(repositoryString.toLocal8Bit());
    repo.close();
    cacheEnabledRepositories();
}
void MeeShop::PackageManager::disableRepository(QString name) {
    QFile::remove(QString("/etc/apt/sources.list.d/meeshop-%1.list").arg(name));
    cacheEnabledRepositories();
}

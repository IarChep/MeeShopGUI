#include "PackageManager.h"


bool MeeShop::PackageManager::is_installed(QString package) {
    qDebug() << "Checking package" << package;;
    return (installedPackages[package].toMap()["Status"] == "install ok installed");
}

void MeeShop::PackageManager::update_repositories() {
    apt.updateRepos();
}

void MeeShop::PackageManager::install_package(QString package) {
    qDebug() << "installing package " << package;
    apt.installPackage(package.toStdString());
}

void MeeShop::PackageManager::cacheInstalledApplications() {
    installedPackages.clear();
    QDBusMessage msg = QDBusMessage::createMethodCall(PKG_SERVICE, PKG_PATH, PKG_IFACE, "fetch_installed");
    QDBusReply<QDBusArgument> reply = bus.call(msg, QDBus::Block, 60000);
    if (reply.isValid()) {
        QDBusArgument var = reply.value();
        var.beginArray();
        while (!var.atEnd()) {
            QVariantMap package;
            var >> package;
            QString packageName = package.value("Name").toString();
            if (!packageName.isEmpty()) {
                installedPackages[packageName] = package;
                qDebug() << "Added package to map with key:" << packageName;
            } else {
                qWarning() << "Package without a name encountered, skipping.";
            }
        }
    } else {
        qWarning() << "Can't cache installed applications" << reply.error().message();
    }
    QFile("/var/lib/dpkg/lock").remove();
}
void MeeShop::PackageManager::printInstalledPackages() {
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

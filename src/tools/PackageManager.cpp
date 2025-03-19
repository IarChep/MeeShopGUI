#include "PackageManager.h"


bool MeeShop::PackageManager::is_installed(QString package) {
    return true;
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
    QDBusMessage msg = QDBusMessage::createMethodCall(PKG_SERVICE,PKG_PATH,PKG_IFACE,"fetch_installed");
    QDBusReply<QDBusArgument> reply = bus.call(msg, QDBus::Block, 60000);
    if (reply.isValid()) {
        QDBusArgument var = reply.value();
        var.beginArray();
        while( !var.atEnd() ) {
            QVariantMap package;
            var >> package;
            qDebug() << "Recieved new package" << package;
            installedPackages.push_back(package);
        }
    } else {
        qWarning() << "Cant cache installed applications" << reply.error().message();
    }
}


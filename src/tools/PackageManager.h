#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <algorithm>
#include "apt/apt.h"
#include <iostream>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

#define PKG_SERVICE "com.nokia.package_manager"
#define PKG_PATH    "/com/nokia/package_manager"
#define PKG_IFACE   "com.nokia.package_manager"

namespace MeeShop {
class PackageManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = 0): QObject(parent), apt("/usr/bin/aegis-apt-get"), bus(QDBusConnection::systemBus()) {
        connect(&apt, SIGNAL(actionChanged(const QString&)), this, SLOT(handleActionChanged(const QString&)));
        connect(&apt, SIGNAL(progressChanged(const QString&, int)), this, SLOT(handleProgressChanged(const QString&, int)));
        connect(&apt, SIGNAL(errorOrWarning(const QString, const QString&)), this, SLOT(handleErrorOrWarning(const QString&, const QString&)));
        connect(&apt, SIGNAL(exited(int, const QString&)), this, SLOT(handleExited(int, const QString&)));
        //qDebug() << "Package list update connected:" << bus.connect(PKG_SERVICE,PKG_PATH,PKG_IFACE,"package_list_updated",this,SLOT(onPkgPackageListUpdate(bool)));
    }

signals:
    void actionChanged(QString action);
    void actionProgressChanged(QString action, int progress);
    void aptErrorOrWarning(QString type, QString message);
    void aptFinished(int code, QString logs);

public slots:
    void update_repositories();
    void install_package(QString package);
    bool is_installed(QString package);
    void cacheInstalledApplications();
    void printInstalledPackages();

private slots:
    void handleActionChanged(const QString& action) {
        qDebug() << "Action changed: " << action << "\n";
        emit actionChanged(action);
    }

    // Slot to handle progress updates
    void handleProgressChanged(const QString& action, int progress) {
        qDebug() << "Action progress changed: " << action << " " << progress << "%\n";
        emit actionProgressChanged(action, progress);
    }

    // Slot to handle errors or warnings
    void handleErrorOrWarning(const QString& type, const QString& message) {
        qDebug().nospace() << "PackageManager. APT failed with " << type << ": " << message;
        emit aptErrorOrWarning(type, message);
    }

    // Slot to handle process exit
    void handleExited(int code, const QString& output) {
        emit aptFinished(code, output);
        qDebug().nospace() << "Apt finished. Code: " << code << " Output:\n" << output << "\n";
    }
    void onPkgPackageListUpdate(bool updates) {
        if (updates) {
            qDebug() << "Apdating packages list";
            cacheInstalledApplications();
        }
    }
private:
    AptTools apt;
    QDBusConnection bus;
    QVariantMap installedPackages;
    QString lastInstalledPackage;

};

}

#endif // PROCESSMANAGER_H

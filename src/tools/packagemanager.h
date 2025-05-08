#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>
#include <QMetaEnum>
#include <QMetaObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QVariant>
#include <QVariantMap>
#include <QMultiHash>

#include "apt/apt.h"
#include "packageutils.h"

namespace MeeShop {
class PackageManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(InstallationStatus)
public:
    explicit PackageManager(QObject *parent = 0): QObject(parent), apt("/usr/bin/aegis-apt-get") {
        connect(&apt, SIGNAL(actionChanged(const QString&)), this, SLOT(handleActionChanged(const QString&)));
        connect(&apt, SIGNAL(progressChanged(const QString&, int)), this, SLOT(handleProgressChanged(const QString&, int)));
        connect(&apt, SIGNAL(errorOrWarning(const QString, const QString&)), this, SLOT(handleErrorOrWarning(const QString&, const QString&)));
    }
    enum InstallationStatus {
        NotInstalled = 0,
        Installed,
        Updatable
    };

signals:
    void actionChanged(QString action);
    void actionProgressChanged(QString action, int progress);
    void aptErrorOrWarning(QString type, QString message);
    void updateFinished(int code, QString logs);
    void installationFinished(int code, QString logs);
    void removeFinished(int code, QString logs);

public slots:
    void installPackage(QString package);

    void cacheEnabledRepositories();
    bool isRepositoryEnabled(QString name);
    void enableRepository(QString name);
    void disableRepository(QString name);
    void updateRepositories();

    int isInstalled(QString package, QString name);
    void cacheInstalledPackages();

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
private:
    AptTools apt;
    QMultiHash<QString, QVariantMap> installedPackages;
    QVariantMap enabledRepositories;
};

}

#endif // PROCESSMANAGER_H

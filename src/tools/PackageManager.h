#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include <QVariant>
#include <QVariantMap>
#include <QMultiHash>

#include "apt/apt.h"
#include "LambdaSlot.h"
#include <iostream>

#include <string>
#include <vector>

namespace MeeShop {
class PackageManager : public QObject
{
    Q_OBJECT
public:
    explicit PackageManager(QObject *parent = 0): QObject(parent), apt("/usr/bin/aegis-apt-get") {
        repoRoot = "/etc/apt/sources.list.d/";
        connect(&apt, SIGNAL(actionChanged(const QString&)), this, SLOT(handleActionChanged(const QString&)));
        connect(&apt, SIGNAL(progressChanged(const QString&, int)), this, SLOT(handleProgressChanged(const QString&, int)));
        connect(&apt, SIGNAL(errorOrWarning(const QString, const QString&)), this, SLOT(handleErrorOrWarning(const QString&, const QString&)));
    }

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

    QString isInstalled(QString package, QString name);
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

    QMultiHash<QString, QVariantMap> parsePkgDatabase(const  QString filePath);
    QString findMaxVersion(const QString packageName, const QString filePath);

    int compareVersions(const std::string& v1, const std::string& v2);
    std::vector<std::string> splitVersion(const std::string& version);

    QString repoRoot;
    QString getRepoPath(const QString name);
    QString getRepoPackagesPath(const QString name);
};

}

#endif // PROCESSMANAGER_H

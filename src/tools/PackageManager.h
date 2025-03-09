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

namespace MeeShop {
class PackageManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = 0) : QObject(parent), apt("/usr/bin/aegis-apt-get") {
        connect(&apt, SIGNAL(actionChanged(const QString&)), this, SLOT(handleActionChanged(const QString& action)));
        connect(&apt, SIGNAL(progressChanged(const QString&, int)), this, SLOT(handleProgressChanged(const QString&, int)));
        connect(&apt, SIGNAL(errorOrWarning(const QString, const QString&)), this, SLOT(handleErrorOrWarning(const QString&, const QString&)));
        connect(&apt, SIGNAL(exited(int code, const QString&)), this, SLOT(handleExited(int code, const QString&)));
    }

    static void install_repo();
    void check_root();

signals:
    void actionChanged(QString action);
    void actionProgressChanged(QString action, int progress);
    void aptErrorOrWarning(QString type, QString message);
    void updateFinished();
    void installationFinished();

public slots:
    void update_repositories();
    void install_package(QString package);
    bool is_installed(QString package);

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
        qDebug().nospace() << "Apt finished. Code: " << code << " Output:\n" << output << "\n";
    }

private:
    AptTools apt;
};

}

#endif // PROCESSMANAGER_H

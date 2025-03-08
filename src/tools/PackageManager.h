#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <algorithm>
#include <QRegExp>
#include "apt/apt.h"
#include <iostream>

namespace MeeShop {
class PackageManager : public QObject
{
    Q_OBJECT

public:
    explicit PackageManager(QObject *parent = 0): QObject(parent), apt("/usr/bin/aegis-apt-get") {
        apt.setOnActionChanged([this](const std::string& action) {
            std::cout << "Action changed: " << action << "\n";
            this->actionChanged(QString::fromStdString(action));
        });
        apt.setOnProgressChanged([this](const std::string action, int progress) {
            std::cout << "Action progress changed: " << action << ' ' << progress << '%' << "\n";
            this->actionProgressChanged(QString::fromStdString(action), progress);
        });
        apt.setOnErrorOrWarning([this](const std::string& type, const std::string& message) {
            if (type == "Error")
                this->aptFailed(QString::fromStdString(message));
        });
        apt.setOnExited([](int code, const std::string& apt_out){
            qDebug().nospace() << "Apt finished. Code: " << code << " Output:\n" << QString::fromStdString(apt_out) << "\n";
        });
    }

    static void install_repo();
    void check_root();
signals:
    void actionChanged(QString action);
    void actionProgressChanged(QString action, int progress);
    void aptFailed(QString message);
    void updateFinished();
    void installationFinished();
public slots:
    void update_repositories();
    void install_package(QString package);
    bool is_installed(QString package);

private:
    AptTools apt;
};

}

#endif // PROCESSMANAGER_H

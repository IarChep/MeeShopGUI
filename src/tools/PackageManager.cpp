#include "PackageManager.h"

void MeeShop::PackageManager::install_repo() {
    QString repo_dir = "/etc/apt/sources.list.d/wunderw-openrepos.list";
    QString pref_dir = "/etc/apt/preferences.d/wunderw-openrepos.pref";
    QFile repo(repo_dir);
    QFile pref(pref_dir);
    if (!(repo.exists() && pref.exists())) {
        if (repo.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream repo_text(&repo);
            repo_text << "deb http://wunderwungiel.pl/MeeGo/openrepos ./";
            repo.close();
            qDebug() << "Repo file created sucsessfully";
        } else {
            qDebug() << "Failed to create the repo file";
        }
        if (pref.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream pref_text(&pref);
            pref_text << "Package: *\nPin: origin wunderw-openrepos\nPin-Priority: 1";
            pref.close();
            qDebug() << "Pref file created successfully.";
        } else {
            qDebug() << "Failed to create the pref file.";
        }
    } else {
        qDebug() << "repo and pref files are already exist";
    }
}
void MeeShop::PackageManager::check_root() {
    QProcess process;
    process.start("/usr/bin/whoami");
    process.waitForFinished();

    QString output(process.readAllStandardOutput());
    qDebug() << "User is" << output;
}

bool MeeShop::PackageManager::is_installed(QString package) {
    return apt.isInstalled(package.toStdString());
}

void MeeShop::PackageManager::update_repositories() {
    apt.updateRepos();
}

void MeeShop::PackageManager::install_package(QString package) {
    qDebug() << "installing package " << package;
    apt.installPackage(package.toStdString());
}

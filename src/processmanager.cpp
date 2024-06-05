#include "processmanager.h"

void MeeShop::ProcessManager::install_repo() {
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
void MeeShop::ProcessManager::check_root() {
    QProcess process;
    process.start("/usr/bin/whoami");
    process.waitForFinished();

    QString output(process.readAllStandardOutput());
    qDebug() << "User is" << output;
}

bool MeeShop::ProcessManager::is_installed(QString package) {
    QProcess process;
    process.start("dpkg", QStringList() << "-s" << package);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output.contains("install ok installed");
}

void MeeShop::ProcessManager::update_repositories() {
    update_process.start("/usr/bin/expect", QStringList() << "-c" << "spawn apt-get update; set progress_re {([0-9]+% \[.*\])}; expect { -re $progress_re { puts \"\n\$expect_out(0,string)\"; exp_continue } eof { exit } }");
}
// some functions to read commmand states properly
void MeeShop::ProcessManager::process_update_output() {
    QRegExp rx("([0-9]+% \\[.*\\])");
    int pos = 0;
    update_output = QString::fromLocal8Bit(update_process.readAllStandardOutput()).simplified();
    while ((pos = rx.indexIn(update_output, pos)) != -1) {
            QString match = rx.cap(1); // Сохраняем найденный процент обработки
            int percentage_pos = filter_percentage.indexIn(match);
                if (percentage_pos > -1) {
                    update_percentage = filter_percentage.cap(1).toInt();
                }
            update_output.remove(match); // Удаляем процент обработки из строки
            pos += rx.matchedLength();
    }
    emit update_percentage_changed();
    emit update_output_changed();
}

void MeeShop::ProcessManager::process_update_error(QProcess::ProcessError error) {
    qDebug() << "Somehow, got into error" << error;
    emit update_finished(false);
}

void MeeShop::ProcessManager::process_update_read_error() {
    update_error = update_process.readAllStandardError();
    emit update_error_changed();
}
void MeeShop::ProcessManager::update_process_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    update_output = "Repositories update is finished!";
    emit update_output_changed();
    emit update_finished(exitStatus == QProcess::NormalExit && exitCode == 0);
}

void MeeShop::ProcessManager::install_package(QString package) {
    install_process.start("aegis-apt-get", QStringList() <<  "install" << "-y" << "--force-yes" << package);
}

void MeeShop::ProcessManager::process_installation_output() {
    installation_output = QString::fromLocal8Bit(install_process.readAllStandardOutput());
    qDebug() << installation_output;
    emit installation_output_changed();
}
void MeeShop::ProcessManager::process_installation_error(QProcess::ProcessError error) {
    emit installation_finished(false);
}
void MeeShop::ProcessManager::process_installation_read_error() {
    installation_error = install_process.readAllStandardError();
    emit installation_error_changed();
}
void MeeShop::ProcessManager::installation_process_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    installation_output= "Application is installed!";
    emit installation_output_changed();
    emit installation_finished(exitStatus == QProcess::NormalExit && exitCode == 0);
}

#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QRegExp>

namespace MeeShop {
class ProcessManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString updateOutput READ get_update_output() NOTIFY update_output_changed)
    Q_PROPERTY(QString updateError READ get_update_error() NOTIFY update_error_changed)
    Q_PROPERTY(int updatePercentage READ get_update_percentage() NOTIFY update_percentage_changed)

    Q_PROPERTY(QString installationOutput READ get_installation_output() NOTIFY installation_output_changed)
    Q_PROPERTY(QString installationError READ get_installation_error() NOTIFY installation_error_changed)
    Q_PROPERTY(QString installationPercentage READ get_installation_percentage() NOTIFY installation_percentage_changed)
public:
    explicit ProcessManager(QObject *parent = 0): QObject(parent), filter_percentage("^([0-9]{1,2}|100)(?=%)") {
        QObject::connect(&update_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(update_process_finished(int,QProcess::ExitStatus)));
        QObject::connect(&update_process, SIGNAL(readyReadStandardOutput()), this, SLOT(process_update_output()));
        QObject::connect(&update_process, SIGNAL(readyReadStandardError()), this, SLOT(process_update_read_error()));
        QObject::connect(&update_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(process_update_error(QProcess::ProcessError)));

        QObject::connect(&install_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(installation_process_finished(int,QProcess::ExitStatus)));
        QObject::connect(&install_process, SIGNAL(readyReadStandardOutput()), this, SLOT(process_installation_output()));
        QObject::connect(&install_process, SIGNAL(readyReadStandardError()), this, SLOT(process_installation_read_error()));
        QObject::connect(&install_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(process_installation_error(QProcess::ProcessError)));
    }

    //Q_PROPERTY functions
    QString get_update_output() {return update_output;}
    QString get_update_error() {return update_error;}
    int get_update_percentage() {return update_percentage;}

    QString get_installation_output() {return installation_output;}
    QString get_installation_error() {return installation_error;}
    int get_installation_percentage() {return installation_percentage;}
    // end Q_PROPERTY functions

    void install_repo();
signals:
    void update_finished(bool sucsess);
    void installation_finished(bool sucsess);

    void update_output_changed();
    void update_error_changed();
    void update_percentage_changed();

    void installation_output_changed();
    void installation_error_changed();
    void installation_percentage_changed();
public slots:
    void update_repositories();
    void install_package(QString package);
    bool is_installed(QString package);

    void reset(){update_output.clear(); update_error.clear(); update_percentage = 0; installation_output.clear(); installation_error.clear(); installation_percentage = 0;}
private slots:
    void update_process_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void process_update_output();
    void process_update_read_error();
    void process_update_error(QProcess::ProcessError error);

    void installation_process_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void process_installation_output();
    void process_installation_read_error();
    void process_installation_error(QProcess::ProcessError error);
private:
    QProcess update_process;
    QProcess install_process;
    QRegExp filter_percentage;

    QString update_output;
    QString update_error;
    int update_percentage;

    QString installation_output;
    QString installation_error;
    int installation_percentage;
};

}

#endif // PROCESSMANAGER_H

#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <QObject>
#include <QRegExp>
#include <QProcess>


class PackageManager : public QObject
{
    Q_OBJECT
public:
    explicit PackageManager(QObject *parent = 0):  QObject(parent),  rx("\\[(\\d+)\\%]") {}
signals:
    void updateFinished(bool sucsess);
    void installationFinished(bool sucsess);
    void repositoryInstalled();

    void updateStatusChanged();
    void installationStatusChanged();

    void updatePercentageChanged();
    void installationPercentageChanged();
private:
    QRegExp rx;
    QProcess process;

    QString installationStatus;
    QString updateStatus;

    int updatePercentage;
    int installationPercentage;
};

#endif // PACKAGEMANAGER_H

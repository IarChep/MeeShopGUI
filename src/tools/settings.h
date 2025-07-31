#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QStringList>
#include <QObject>

class Settings : public QObject {
    Q_OBJECT

public:
    static Settings& getInstance() {
        static Settings instance;
        return instance;
    }
    QSettings& getSettings() {return m_qsettings;}

    Q_INVOKABLE bool isFirstLaunch() {
        return m_qsettings.value("firstLaunch", true).toBool();
    }
    Q_INVOKABLE void firstLaunchCompleted() {
        m_qsettings.setValue("firstLaunch", false);
    }

    Q_INVOKABLE void addMeeshopPackage(QString package) {
        m_meeshopPackages.push_back(package);
        m_qsettings.setValue("applications/meeshopPackages", m_meeshopPackages);
    }

    Q_INVOKABLE QStringList& getMeeshopPackages()  {
        return m_meeshopPackages;
    }
private:
    explicit Settings(QObject *parent = nullptr) : m_qsettings("IarChep", "MeeShop", parent)
    {
        m_meeshopPackages = m_qsettings.value("applications/meeshopPackages", QStringList()).toStringList();
    }

    QSettings m_qsettings;
    QStringList m_meeshopPackages;
};



#endif // SETTINGS_H

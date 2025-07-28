#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
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
private:
    explicit Settings(QObject *parent = nullptr) : m_qsettings("IarChep", "MeeShop", parent)
    {}

    QSettings m_qsettings;
};



#endif // SETTINGS_H

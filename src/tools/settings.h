#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QStringList>
#include <QObject>
#include <QVariant>
#include <QVariantMap>

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

    // Записать установленное через MeeShop приложение: имя пакета + URL иконки
    // (чтобы страница установленных показывала ту же иконку, что и в магазине).
    Q_INVOKABLE void addMeeshopApp(QString package, QString iconUrl) {
        if (package.isEmpty())
            return;
        bool changed = false;
        // Не дублируем: повторная установка того же пакета не плодит записи.
        if (!m_meeshopPackages.contains(package)) {
            m_meeshopPackages.push_back(package);
            m_qsettings.setValue("applications/meeshopPackages", m_meeshopPackages);
            changed = true;
        }
        if (!iconUrl.isEmpty() && m_icons.value(package).toString() != iconUrl) {
            m_icons[package] = iconUrl;
            m_qsettings.setValue("applications/meeshopIcons", m_icons);
            changed = true;
        }
        // ВАЖНО: sync() — иначе при «убийстве» приложения (свайп-закрытие на N9)
        // QSettings не успевает сбросить буфер на диск и список «обнуляется» при
        // следующем запуске. Принудительно фиксируем запись сразу.
        if (changed)
            m_qsettings.sync();
    }

    Q_INVOKABLE void addMeeshopPackage(QString package) {
        addMeeshopApp(package, QString());
    }

    // Убрать пакет из списка установленных через MeeShop (после успешного удаления).
    Q_INVOKABLE void removeMeeshopPackage(QString package) {
        bool changed = false;
        if (m_meeshopPackages.removeAll(package) > 0) {
            m_qsettings.setValue("applications/meeshopPackages", m_meeshopPackages);
            changed = true;
        }
        if (m_icons.remove(package) > 0) {
            m_qsettings.setValue("applications/meeshopIcons", m_icons);
            changed = true;
        }
        if (changed)
            m_qsettings.sync();
    }

    Q_INVOKABLE QStringList getMeeshopPackages() const {
        return m_meeshopPackages;
    }

    // Сохранённый URL иконки приложения (пусто, если не сохраняли).
    Q_INVOKABLE QString iconForPackage(QString package) const {
        return m_icons.value(package).toString();
    }
private:
    explicit Settings(QObject *parent = nullptr) : m_qsettings("IarChep", "MeeShop", parent)
    {
        m_meeshopPackages = m_qsettings.value("applications/meeshopPackages", QStringList()).toStringList();
        m_icons = m_qsettings.value("applications/meeshopIcons").toMap();
    }

    QSettings m_qsettings;
    QStringList m_meeshopPackages;
    QVariantMap m_icons; // package -> iconUrl (хранится одним значением QSettings)
};



#endif // SETTINGS_H

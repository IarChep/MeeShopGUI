#ifndef MEESHOP_PACKAGEUTILS_H
#define MEESHOP_PACKAGEUTILS_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QMultiHash>
#include <QVariant>
#include <QVariantMap>
#include <QDebug>
#include <QRegExp>
#include <QProcess>

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

#include "settings.h"

#include <nlohmann/json.hpp>


namespace MeeShop {

class PackageUtils : public QObject
{
    Q_OBJECT
public:
    explicit PackageUtils(QObject *parent = nullptr, Settings& settings = Settings::getInstance()) : m_qsettings(settings) {}

    static QMultiHash<QString, QVariantMap> parsePkgDatabase(const  QString filePath);
    static QString findMaxVersion(const QString packageName, const QString filePath);
    static QString findMaxVersion(const QString packageName, const QMultiHash<QString, QVariantMap> &packages);

    static int compareVersions(const std::string& v1, const std::string& v2);
    static std::vector<std::string> splitVersion(const std::string& version);

    static QString getRepoPath(const QString name);
    static QString getRepoPackagesPath(const QString name);

    Q_INVOKABLE bool isRunnable(const QString package);
    Q_INVOKABLE void run(const QString package);
    //Q_INVOKABLE  QStringList getMeeShopInstalledApps();
private:
    nlohmann::json m_cacheJson;
    Settings& m_qsettings;
};

} // namespace MeeShop

#endif // MEESHOP_PACKAGEUTILS_H

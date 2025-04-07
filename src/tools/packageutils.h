#ifndef MEESHOP_PACKAGEUTILS_H
#define MEESHOP_PACKAGEUTILS_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMultiHash>
#include <QVariant>
#include <QVariantMap>
#include <QDebug>
#include <QRegExp>

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>


namespace MeeShop {

class PackageUtils : public QObject
{
    Q_OBJECT
public:
    explicit PackageUtils(QObject *parent = nullptr) : QObject{parent}
    {}

    static QMultiHash<QString, QVariantMap> parsePkgDatabase(const  QString filePath);
    static QString findMaxVersion(const QString packageName, const QString filePath);

    static int compareVersions(const std::string& v1, const std::string& v2);
    static std::vector<std::string> splitVersion(const std::string& version);

    static QString getRepoPath(const QString name);
    static QString getRepoPackagesPath(const QString name);
};

} // namespace MeeShop

#endif // MEESHOP_PACKAGEUTILS_H

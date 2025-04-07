#include "packageutils.h"

namespace MeeShop {

QMultiHash<QString, QVariantMap> PackageUtils::parsePkgDatabase(QString filePath) {
    QMultiHash<QString, QVariantMap> packages;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не могу открыть файл:" << filePath;
        return packages;
    }

    QTextStream in(&file);
    QVariantMap currentPackage;
    QString currentPackageName;
    bool inPackageBlock = false;

    QRegExp packageRx("^Package:\\s*(\\S+)");
    QRegExp fieldRx("^([\\w-]+):\\s*(.*)");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (packageRx.indexIn(line) != -1) {
            if (inPackageBlock && !currentPackageName.isEmpty()) {
                packages.insert(currentPackageName, currentPackage); // Добавляем пакет
            }
            currentPackageName = packageRx.cap(1);
            currentPackage.clear();
            inPackageBlock = true;
        } else if (inPackageBlock && !line.isEmpty()) {
            if (fieldRx.indexIn(line) != -1) {
                QString fieldName = fieldRx.cap(1);
                QString fieldValue = fieldRx.cap(2).trimmed();
                currentPackage[fieldName] = fieldValue;
            }
        } else if (inPackageBlock && line.isEmpty()) {
            packages.insert(currentPackageName, currentPackage); // Добавляем пакет
            currentPackageName.clear();
            currentPackage.clear();
            inPackageBlock = false;
        }
    }

    if (inPackageBlock && !currentPackageName.isEmpty()) {
        packages.insert(currentPackageName, currentPackage); // Добавляем последний пакет
    }

    file.close();
    return packages;
}

QString PackageUtils::getRepoPath(QString name) {
    return QString("/etc/apt/sources.list.d/meeshop-%1.list").arg(name);
}

QString PackageUtils::getRepoPackagesPath(QString name) {
    return QString("/var/lib/apt/lists/harmattan.openrepos.net_%1_dists_personal_main_binary-armel_Packages").arg(name);
}

QString PackageUtils::findMaxVersion(const QString packageName, const QString filePath) {
    QMultiHash<QString, QVariantMap> packages = parsePkgDatabase(filePath);

    if (!packages.contains(packageName)) {
        qWarning() << "Пакет" << packageName << "не найден в файле" << filePath;
        return QString();
    }

    QList<QVariantMap> versions = packages.values(packageName);
    if (versions.isEmpty()) {
        return QString();
    }

    QString maxVersion = versions.first()["Version"].toString();
    for (const QVariantMap& pkg : versions) {
        QString version = pkg["Version"].toString();
        qDebug() << "Found version" << version << "for" << packageName << "in repository" << "\n versionLess than:" << maxVersion << compareVersions(maxVersion.toStdString(), version.toStdString());
        if (compareVersions(maxVersion.toStdString(), version.toStdString()) < 0) {
            maxVersion = version;
        }
    }
    qDebug() << "maxVersion for" << packageName << "is" << maxVersion;
    return maxVersion;
}

std::vector<std::string> PackageUtils::splitVersion(const std::string& version) {
    std::vector<std::string> parts;
    std::string current;
    for (char c : version) {
        if (std::isdigit(c)) {
            if (!current.empty() && !std::isdigit(current.back())) {
                parts.push_back(current);
                current.clear();
            }
            current += c;
        } else {
            if (!current.empty() && std::isdigit(current.back())) {
                parts.push_back(current);
                current.clear();
            }
            current += c;
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }
    return parts;
}

int PackageUtils::compareVersions(const std::string& v1, const std::string& v2) {
    size_t colonPos1 = v1.find(':');
    size_t colonPos2 = v2.find(':');
    int epoch1 = (colonPos1 != std::string::npos) ? std::stoi(v1.substr(0, colonPos1)) : 0;
    int epoch2 = (colonPos2 != std::string::npos) ? std::stoi(v2.substr(0, colonPos2)) : 0;

    std::string upstream1 = (colonPos1 != std::string::npos) ? v1.substr(colonPos1 + 1) : v1;
    std::string upstream2 = (colonPos2 != std::string::npos) ? v2.substr(colonPos2 + 1) : v2;

    size_t hyphenPos1 = upstream1.rfind('-');
    size_t hyphenPos2 = upstream2.rfind('-');
    std::string revision1 = (hyphenPos1 != std::string::npos) ? upstream1.substr(hyphenPos1 + 1) : "";
    std::string revision2 = (hyphenPos2 != std::string::npos) ? upstream2.substr(hyphenPos2 + 1) : "";

    std::string upstreamPart1 = (hyphenPos1 != std::string::npos) ? upstream1.substr(0, hyphenPos1) : upstream1;
    std::string upstreamPart2 = (hyphenPos2 != std::string::npos) ? upstream2.substr(0, hyphenPos2) : upstream2;

    if (epoch1 < epoch2) return -1;
    if (epoch1 > epoch2) return 1;

    std::vector<std::string> parts1 = splitVersion(upstreamPart1);
    std::vector<std::string> parts2 = splitVersion(upstreamPart2);

    size_t maxLen = std::max(parts1.size(), parts2.size());
    for (size_t i = 0; i < maxLen; ++i) {
        std::string p1 = (i < parts1.size()) ? parts1[i] : "";
        std::string p2 = (i < parts2.size()) ? parts2[i] : "";
        if (!std::all_of(p1.begin(), p1.end(), ::isdigit) && !std::all_of(p2.begin(), p2.end(), ::isdigit)) {
            if (p1 < p2) return -1;
            if (p1 > p2) return 1;
        } else if (std::all_of(p1.begin(), p1.end(), ::isdigit) && std::all_of(p2.begin(), p2.end(), ::isdigit)) {
            int num1 = std::stoi(p1.empty() ? "0" : p1);
            int num2 = std::stoi(p2.empty() ? "0" : p2);
            if (num1 < num2) return -1;
            if (num1 > num2) return 1;
        } else if (std::all_of(p1.begin(), p1.end(), ::isdigit)) {
            return 1;
        } else if (std::all_of(p2.begin(), p2.end(), ::isdigit)) {
            return -1;
        }
    }

    std::vector<std::string> revParts1 = splitVersion(revision1);
    std::vector<std::string> revParts2 = splitVersion(revision2);

    maxLen = std::max(revParts1.size(), revParts2.size());
    for (size_t i = 0; i < maxLen; ++i) {
        std::string p1 = (i < revParts1.size()) ? revParts1[i] : "";
        std::string p2 = (i < revParts2.size()) ? revParts2[i] : "";
        if (!std::all_of(p1.begin(), p1.end(), ::isdigit) && !std::all_of(p2.begin(), p2.end(), ::isdigit)) {
            if (p1 < p2) return -1;
            if (p1 > p2) return 1;
        } else if (std::all_of(p1.begin(), p1.end(), ::isdigit) && std::all_of(p2.begin(), p2.end(), ::isdigit)) {
            int num1 = std::stoi(p1.empty() ? "0" : p1);
            int num2 = std::stoi(p2.empty() ? "0" : p2);
            if (num1 < num2) return -1;
            if (num1 > num2) return 1;
        } else if (std::all_of(p1.begin(), p1.end(), ::isdigit)) {
            return 1;
        } else if (std::all_of(p2.begin(), p2.end(), ::isdigit)) {
            return -1;
        }
    }
    return 0;
}

} // namespace MeeShop

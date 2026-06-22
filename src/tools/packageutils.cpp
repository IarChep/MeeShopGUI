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
    return findMaxVersion(packageName, parsePkgDatabase(filePath));
}

QString PackageUtils::findMaxVersion(const QString packageName, const QMultiHash<QString, QVariantMap> &packages) {
    if (!packages.contains(packageName)) {
        qWarning() << "Пакет" << packageName << "не найден";
        return QString();
    }

    QList<QVariantMap> versions = packages.values(packageName);
    if (versions.isEmpty()) {
        return QString();
    }

    QString maxVersion = versions.first()["Version"].toString();
    for (const QVariantMap& pkg : versions) {
        QString version = pkg["Version"].toString();
        if (compareVersions(maxVersion.toStdString(), version.toStdString()) < 0) {
            maxVersion = version;
        }
    }
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

namespace {
// Сравнивает разбитые на токены части версии (upstream или revision).
// Числовые токены сравниваются как числа, остальные — лексикографически;
// числовой токен старше нечислового. Пустая строка трактуется как 0.
int compareParts(const std::vector<std::string> &a, const std::vector<std::string> &b) {
    size_t maxLen = std::max(a.size(), b.size());
    for (size_t i = 0; i < maxLen; ++i) {
        std::string p1 = (i < a.size()) ? a[i] : "";
        std::string p2 = (i < b.size()) ? b[i] : "";
        bool num1 = std::all_of(p1.begin(), p1.end(), ::isdigit);
        bool num2 = std::all_of(p2.begin(), p2.end(), ::isdigit);
        if (!num1 && !num2) {
            if (p1 < p2) return -1;
            if (p1 > p2) return 1;
        } else if (num1 && num2) {
            int n1 = 0, n2 = 0;
            try { n1 = std::stoi(p1.empty() ? "0" : p1); } catch (...) {}
            try { n2 = std::stoi(p2.empty() ? "0" : p2); } catch (...) {}
            if (n1 < n2) return -1;
            if (n1 > n2) return 1;
        } else {
            return num1 ? 1 : -1;
        }
    }
    return 0;
}

int parseEpoch(const std::string &v, size_t colonPos) {
    if (colonPos == std::string::npos) return 0;
    try { return std::stoi(v.substr(0, colonPos)); } catch (...) { return 0; }
}
}

int PackageUtils::compareVersions(const std::string& v1, const std::string& v2) {
    size_t colonPos1 = v1.find(':');
    size_t colonPos2 = v2.find(':');
    int epoch1 = parseEpoch(v1, colonPos1);
    int epoch2 = parseEpoch(v2, colonPos2);
    if (epoch1 != epoch2) return epoch1 < epoch2 ? -1 : 1;

    std::string upstream1 = (colonPos1 != std::string::npos) ? v1.substr(colonPos1 + 1) : v1;
    std::string upstream2 = (colonPos2 != std::string::npos) ? v2.substr(colonPos2 + 1) : v2;

    size_t hyphenPos1 = upstream1.rfind('-');
    size_t hyphenPos2 = upstream2.rfind('-');
    std::string revision1 = (hyphenPos1 != std::string::npos) ? upstream1.substr(hyphenPos1 + 1) : "";
    std::string revision2 = (hyphenPos2 != std::string::npos) ? upstream2.substr(hyphenPos2 + 1) : "";

    std::string upstreamPart1 = (hyphenPos1 != std::string::npos) ? upstream1.substr(0, hyphenPos1) : upstream1;
    std::string upstreamPart2 = (hyphenPos2 != std::string::npos) ? upstream2.substr(0, hyphenPos2) : upstream2;

    int up = compareParts(splitVersion(upstreamPart1), splitVersion(upstreamPart2));
    if (up != 0) return up;

    return compareParts(splitVersion(revision1), splitVersion(revision2));
}

bool PackageUtils::isRunnable(const QString package) {
    const std::string pkgKey = package.toStdString();
    if (m_cacheJson.contains(pkgKey)) {
        return m_cacheJson[pkgKey]["runnable"].get<bool>();
    }

    auto cacheResult = [&](bool runnable, const QString &desktopPath = QString()) {
        m_cacheJson[pkgKey]["runnable"] = runnable;
        if (!desktopPath.isEmpty()) {
            m_cacheJson[pkgKey]["desktop-file"] = desktopPath.toStdString();
        }
        return runnable;
    };

    QProcess process;

    process.start("/bin/dpkg", QStringList() << "-L" << package);

    if (!process.waitForStarted(3000)) {
        qDebug() << "Failed to start dpkg for package" << package;
        return cacheResult(false);
    }

    QByteArray buffer;
    static const QByteArray applicationPrefix = "/usr/share/applications/";
    static const QByteArray desktopSuffix = ".desktop";

    while (process.state() == QProcess::Running || process.bytesAvailable() > 0) {

        if (!process.waitForReadyRead(500)) {
            if (process.state() != QProcess::Running) {
                break;
            }
            continue;
        }

        QByteArray data = process.readAllStandardOutput();
        buffer.append(data);

        int start = 0;
        int newlinePos;

        while ((newlinePos = buffer.indexOf('\n', start)) != -1) {
            QByteArray line = buffer.mid(start, newlinePos - start);
            start = newlinePos + 1;

            if (line.startsWith(applicationPrefix) && line.endsWith(desktopSuffix)) {
                QString desktopFile = QString::fromUtf8(line).trimmed();
                process.kill();
                process.waitForFinished(1000);
                return cacheResult(true, desktopFile);
            }
        }
        if (start > 0) {
            buffer.remove(0, start);
        }
    }

    if (!buffer.isEmpty()) {
        if (buffer.startsWith(applicationPrefix) && buffer.endsWith(desktopSuffix)) {
            QString desktopFile = QString::fromUtf8(buffer).trimmed();
            return cacheResult(true, desktopFile);
        }
    }

    if (process.exitCode() != 0) {
        qDebug() << "dpkg failed for package" << package
                 << "with exit code" << process.exitCode();
    }

    return cacheResult(false);
}

void PackageUtils::run(const QString package) {
    const std::string key = package.toStdString();
    if (!m_cacheJson.contains(key) || !m_cacheJson[key].contains("desktop-file")) {
        qDebug() << "No cached desktop file for" << package << "- call isRunnable() first";
        return;
    }
    QString prog = "/usr/bin/xdg-open";
    QStringList args;
    args << QString::fromStdString(m_cacheJson[key]["desktop-file"].get<std::string>());
    qint64 pid;
    if(QProcess::startDetached(prog, args, QString(), &pid)) {
        qDebug() << "Sucsessfully ran " << package << " with pid " << pid;
    } else {
        qDebug() << "Failed to run " << package << " with pid " << pid;
    }
}

} // namespace MeeShop

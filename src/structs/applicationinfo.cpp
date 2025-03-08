#include "applicationinfo.h"

namespace MeeShop {

ApplicationInfo::ApplicationInfo(QObject *parent)
    : QObject(parent)
{

}

void ApplicationInfo::setJson(const nlohmann::json app) {
    m_appJson = app;
    qDebug() << QString::fromStdString(m_appJson.dump(4)) << "\n";
    emit appChanged();
}

QString ApplicationInfo::id() const {
    if(m_appJson.contains("appid"))
        return QString::fromStdString(m_appJson["appid"].get<std::string>());
    return QString(); // Возвращаем пустую строку, если ключа нет
}

QString ApplicationInfo::title() const {
    if(m_appJson.contains("title"))
        return QString::fromStdString(m_appJson["title"].get<std::string>());
    return QString();
}

QString ApplicationInfo::publisher() const {
    if(m_appJson.contains("user") && m_appJson["user"].contains("name"))
        return QString::fromStdString(m_appJson["user"]["name"].get<std::string>());
    return QString();
}

QString ApplicationInfo::description() const {
    if(m_appJson.contains("body"))
        return QString::fromStdString(m_appJson["body"].get<std::string>());
    return QString();
}

QString ApplicationInfo::package() const {
    if(m_appJson.contains("packages") && m_appJson["packages"].contains("harmattan") && m_appJson["packages"]["harmattan"].contains("name"))
        return QString::fromStdString(m_appJson["packages"]["harmattan"]["name"].get<std::string>());
    else if(m_appJson.contains("package") && m_appJson["package"].contains("name"))
        return QString::fromStdString(m_appJson["package"]["name"].get<std::string>());
    return QString();
}

QString ApplicationInfo::downloadsCount() const {
    if(m_appJson.contains("downloads"))
        return QString::fromStdString(m_appJson["downloads"].get<std::string>());
    return QString();
}

int ApplicationInfo::ratingCount() const {
    if(m_appJson.contains("rating") && m_appJson["rating"].contains("count"))
        return std::stoi(m_appJson["rating"]["count"].get<std::string>());
    return 0;
}

QStringList ApplicationInfo::screenshotsThumbnails() const {
    QStringList thumbs;
    if(m_appJson.contains("screenshots")) {
        for (const auto& screenshot : m_appJson["screenshots"]) {
            if(screenshot.contains("thumbs") && screenshot["thumbs"].contains("medium"))
                thumbs.push_back(QString::fromStdString(screenshot["thumbs"]["medium"].get<std::string>()));
        }
    }
    return thumbs;
}

QStringList ApplicationInfo::screenshots() const {
    QStringList screenshots;
    if(m_appJson.contains("screenshots")) {
        for (const auto& screenshot : m_appJson["screenshots"]) {
            if(screenshot.contains("url"))
                screenshots.push_back(QString::fromStdString(screenshot["url"].get<std::string>()));
        }
    }
    return screenshots;
}

QString ApplicationInfo::icon() const {
    if(m_appJson.contains("icon") && m_appJson["icon"].contains("url"))
        return QString::fromStdString(m_appJson["icon"]["url"].get<std::string>());
    return QString();
}

} // namespace MeeShop

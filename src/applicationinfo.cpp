#include "applicationinfo.h"

namespace MeeShop {

ApplicationInfo::ApplicationInfo(QObject *parent)
    : QObject{parent}
{

}

void ApplicationInfo::setJson(nlohmann::json app) {
    m_appJson = app;
    emit appChanged();
}

QString ApplicationInfo::id() {
    if(m_appJson.contains("appid"))
        return QString::fromStdString(m_appJson["appid"].get<std::string>());
}
QString ApplicationInfo::title() {
    if(m_appJson.contains("title")) {
        return QString::fromStdString(m_appJson["title"].get<std::string>());
    }
}
QString ApplicationInfo::publisher() {
    if(m_appJson.contains("packages"))
        return QString::fromStdString(m_appJson["packages"]["harmattan"].get<std::string>());
    else if(m_appJson.contains("package"))
        return QString::fromStdString(m_appJson["package"]["name"].get<std::string>());
}
QString ApplicationInfo::description() {
    if(m_appJson.contains("body"))
        return QString::fromStdString(m_appJson["body"].get<std::string>());
}
QString ApplicationInfo::package() {
    if(m_appJson.contains("body"))
        return QString::fromStdString(m_appJson["body"].get<std::string>());
}
QString ApplicationInfo::downloadsCount() {
    if(m_appJson.contains("downloads"))
        return QString::fromStdString(m_appJson["downloads"].get<std::string>());
}
int ApplicationInfo::ratingCount() {
    if(m_appJson.contains("rating"))
        return std::stoi(m_appJson["rating"]["count"].get<std::string>());
}
QStringList ApplicationInfo::screenshotsThumbnails() {
    if(m_appJson.contains("screenshots")) {
        QStringList thumbs;
        for (nlohmann::json &screenshot : m_appJson["screenshots"]) {
            thumbs.push_back(QString::fromStdString(screenshot["thumbs"]["medium"].get<std::string>()));
        }
        return thumbs;
    }
}
QStringList ApplicationInfo::screenshots() {
    if(m_appJson.contains("screenshots")) {
        QStringList screenshots;
        for (nlohmann::json &screenshot : m_appJson["screenshots"]) {
            screenshots.push_back(QString::fromStdString(screenshot["url"].get<std::string>()));
        }
        return screenshots;
    }
}
QString ApplicationInfo::icon() {
    if(m_appJson.contains("icon"))
        return QString::fromStdString(m_appJson["icon"]["url"].get<std::string>());
}

} // namespace MeeShop

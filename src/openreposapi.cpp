#include "openreposapi.h"

namespace MeeShop {

void OpenReposApi::getCategories() {
    currentRoute = "/categories"; // Set the current route for categories
    request.setUrl(QUrl(baseUrl + currentRoute));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::getCategoryApps(int cat_id, int page) {
    currentRoute = "/categories/" + QString::number(cat_id) + "/apps?page=" + QString::number(page); // Set route for category apps
    request.setUrl(QUrl(baseUrl + currentRoute));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::search(QString query) {
    currentRoute = "/search/apps?keys=" + query; // Set route for search
    request.setUrl(QUrl(baseUrl + currentRoute));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::getAppInfo(int app_id) {
    currentRoute = "/apps/" + QString::number(app_id); // Set route for app info
    request.setUrl(QUrl(baseUrl + currentRoute));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::getAppComments(int app_id) {
    currentRoute = "/apps/" + QString::number(app_id) + "/comments"; // Set route for app comments
    request.setUrl(QUrl(baseUrl + currentRoute));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::process_reply() {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        std::string dataStr(data.constData(), data.size());
        if (nlohmann::json::accept(dataStr)) {
            nlohmann::json json_obj = nlohmann::json::parse(dataStr);
            if (currentRoute.startsWith("/categories/")) {
                appModel->pushPageBack(json_obj);
                emit modelChanged();
            } else if (currentRoute.startsWith("/categories")) {
                categoryModel->setJson(json_obj);
                emit catModelChanged();
            }
            emit finished(true);
        }
    } else {
        emit finished(false);
    }
}
}

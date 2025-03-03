#include "openreposapi.h"

namespace MeeShop {

void OpenReposApi::getCategories() {
    QString currentRoute = "/categories"; // Set the current route for categories
    request.setUrl(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(process_categories()));
}

void OpenReposApi::getCategoryApps(int cat_id, int page) {
    currentPage = page;
    QString currentRoute = "/categories/" + QString::number(cat_id) + "/apps?page=" + QString::number(page); // Set route for category apps
    request.setUrl(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(process_apps()));
}

void OpenReposApi::search(QString query) {
    QString currentRoute = "/search/apps?keys=" + query; // Set route for search
    request.setUrl(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);

    //QObject::connect(reply, SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::fetchAppInfo(int app_id) {
    QEventLoop loop;
    QString currentRoute = "/apps/" + QString::number(app_id); // Set route for app info
    request.setUrl(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        nlohmann::json jsonObj = parseJson(reply->readAll());
        appInfo->setJson(jsonObj);
        qDebug() << "changed appInfo";
        emit appInfoChanged();
    } else {
        qDebug() << "something is failed!";
    }
}

void OpenReposApi::getAppComments(int app_id) {
    QString currentRoute = "/apps/" + QString::number(app_id) + "/comments"; // Set route for app comments
    request.setUrl(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);
    //QObject::connect(reply, SIGNAL(finished()), this, SLOT(process_reply()));
}

nlohmann::json OpenReposApi::parseJson(QByteArray data) {
    std::string dataStr(data.constData(), data.size());
    if (nlohmann::json::accept(dataStr)) {
        return nlohmann::json::parse(dataStr);
    } else {
        return {};
    }
}
void OpenReposApi::process_apps() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        nlohmann::json jsonObj = parseJson(reply->readAll());
        if (currentPage >= lastPage)
            appModel->pushPageBack(jsonObj);
        else
            appModel->pushPageFront(jsonObj);
        lastPage = currentPage;
        emit modelChanged();
        emit finished(true);
    } else {
        emit finished(false);
    }
    reply->deleteLater();
}
void OpenReposApi::process_categories() {
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        nlohmann::json jsonObj = parseJson(reply->readAll());
        categoryModel->setJson(jsonObj);
        emit catModelChanged();
        emit finished(true);
    } else {
        emit finished(false);
    }
    reply->deleteLater();
}
}

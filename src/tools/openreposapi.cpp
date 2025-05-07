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

void OpenReposApi::getApplication(int app_id) {
    QString currentRoute = "/apps/" + QString::number(app_id); // Set route for app info
    request.setUrl(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(process_app()));
}

void OpenReposApi::getAppComments(int app_id) {
    QString currentRoute = "/apps/" + QString::number(app_id) + "/comments"; // Set route for app comments
    request.setUrl(QUrl(baseUrl + currentRoute));
    QNetworkReply *reply = manager.get(request);
    //QObject::connect(reply, SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::process_apps() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        nlohmann::json jsonObj = parseJson(reply->readAll());
        if (currentPage >= lastPage)
            appModel->pushPageBack(jsonObj);
        else
            appModel->pushPageFront(jsonObj);
        lastPage = currentPage;
        emit appModelChanged();
        emit finished(true);
    } else {
        emit finished(false);
    }
    reply->deleteLater();
}
void OpenReposApi::process_categories() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        nlohmann::json jsonObj = parseJson(reply->readAll());
        categoryModel->setJson(jsonObj);
        emit categoryModelChanged();
        emit finished(true);
    } else {
        emit finished(false);
    }
    reply->deleteLater();
}

void OpenReposApi::process_app() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "changed appInfo";
        nlohmann::json json = parseJson(reply->readAll());
        qDebug() << json.dump(4).c_str();
        QVariant var = jsonToVariant(json);
        appInfo = var.toMap();
        emit appInfoChanged();
    } else {
        qDebug() << "something is failed!";
    }
    reply->deleteLater();
}

nlohmann::json OpenReposApi::parseJson(const QByteArray& data) {
    std::string dataStr(data.constData(), data.size());
    if (nlohmann::json::accept(dataStr)) {
        return nlohmann::json::parse(dataStr);
    } else {
        return {};
    }
}
QVariant OpenReposApi::jsonToVariant(const nlohmann::json &j) {
    if (j.is_object()) {
        QVariantMap map;
        for (auto it = j.begin(); it != j.end(); ++it) {
            QString key = QString::fromStdString(it.key());
            map.insert(key, jsonToVariant(it.value()));
        }
        return map;
    } else if (j.is_array()) {
        QVariantList list;
        for (const auto &item : j) {
            list.append(jsonToVariant(item));
        }
        return list;
    } else if (j.is_string()) {
        return QString::fromStdString(j.get<std::string>());
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<unsigned int>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_null()) {
        return QVariant();
    }

    return QVariant();
}
}

#include "openreposapi.h"

namespace MeeShop {

void OpenReposApi::getCategories() {
    request.setUrl(QUrl("https://openrepos.net/api/v1/categories"));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}
void OpenReposApi::getCategoryApps(int cat_id) {
    request.setUrl(QUrl("https://openrepos.net/api/v1/categories/" + QString::number(cat_id) + "/apps"));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}
void OpenReposApi::search(QString query) {
    request.setUrl(QUrl("https://openrepos.net/api/v1/search/apps?keys=" + query));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}
void OpenReposApi::getAppInfo(int app_id) {
    request.setUrl(QUrl("https://openrepos.net/api/v1/apps/" + QString::number(app_id)));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}
void OpenReposApi::getAppComments(int app_id) {
    request.setUrl(QUrl("https://openrepos.net/api/v1/apps/" + QString::number(app_id) + "/comments"));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void OpenReposApi::process_reply() {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        std::string dataStr(data.constData(), data.size());
        if (nlohmann::json::accept(dataStr)) {
            nlohmann::json json_obj = nlohmann::json::parse(dataStr);
            categoryModel->setJson(json_obj);
            emit catModelChanged();
        }
        emit finished(true);
    } else {
        emit finished(false);
    }
}
}

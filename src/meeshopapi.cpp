#include "meeshopapi.h"

namespace MeeShop {
    MeeShopApi::MeeShopApi(QObject *parent)
        : QObject{parent}, appModel(new MeeShop::MeeShopApplicationModel(this)), categories(false)
    {}

    void MeeShopApi::getCategories() {
        categories = true;
        QNetworkRequest request(QUrl("http://wunderwungiel.pl/meeshop/api/categories"));
        reply.reset(manager.get(request));
        QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(processReply()));
    }
    void MeeShopApi::getCategoryContent(int catId, int pageNum, QString sortBy) {
        QNetworkRequest request(QUrl("http://wunderwungiel.pl/meeshop/api/categories/"+ QString::number(catId)+"/content"));
        request.setRawHeader("Count",QString::number(10).toUtf8());
        request.setRawHeader("Page", QString::number(pageNum).toUtf8());
        request.setRawHeader("Sort-By", sortBy.toUtf8());
        reply.reset(manager.get(request));
        QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(processReply()));
    }
    int MeeShopApi::getPages(int catId) {
        QNetworkRequest request(QUrl("http://wunderwungiel.pl/meeshop/api/categories/"+ QString::number(catId)+"/pages"));
        request.setRawHeader("Count",QString::number(10).toUtf8());
        reply.reset(manager.get(request));
        QObject::connect(reply.data(), SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        QByteArray data = reply->readAll();
        std::string jsonString(data.constData(), data.size());
        nlohmann::json parsed = nlohmann::json::parse(jsonString);
        return parsed["pages"].get<int>();
    }
    void MeeShopApi::getTop() {
        QNetworkRequest request(QUrl("http://wunderwungiel.pl/meeshop/api/content/top"));
        request.setRawHeader("count", QString::number(10).toUtf8());
        reply.reset(manager.get(request));
        QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(processReply()));
    }
    void MeeShopApi::addDownload(QString packageName) {
        QNetworkRequest request(QUrl("http://wunderwungiel.pl/meeshop/api/content/add_download"));
        request.setRawHeader("package", packageName.toUtf8());
        reply.reset(manager.get(request));
    }
    void MeeShopApi::search(QString query, int catId, int pageNum) {
        QNetworkRequest request(QUrl("http://wunderwungiel.pl/meeshop/api/search"));
        request.setRawHeader("count", QString::number(10).toUtf8());
        request.setRawHeader("page", QString::number(pageNum).toUtf8());
        request.setRawHeader("query", query.toUtf8());
        request.setRawHeader("category", QString::number(catId).toUtf8());
        reply.reset(manager.get(request));
        QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(processReply()));
    }

    void MeeShopApi::processReply() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            std::string jsonString(data.constData(), data.size());
            nlohmann::json parsed = nlohmann::json::parse(jsonString);
            if (categories) {
                categories = false;
                categoryModel = new MeeShop::MeeShopCategoriesModel(this);                
                categoryModel->setJson(parsed);
                emit catModelChanged();
                emit finished(true);
                return;
            }
            appModel = new MeeShop::MeeShopApplicationModel(this);
            appModel->setJson(parsed);
            emit modelChanged();
            emit finished(true);
        } else {
            emit finished(false);
        }
    }
}

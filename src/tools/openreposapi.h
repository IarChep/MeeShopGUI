#ifndef MEESHOP_OPENREPOSAPI_H
#define MEESHOP_OPENREPOSAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QCoreApplication>
#include <nlohmann/json.hpp>
#include "../models/MeeShopApplicationModel.h"
#include "../models/MeeShopCategoriesModel.h"
#include "../structs/applicationinfo.h"
#include "LambdaSlot.h"
#include <QVariantHash>

namespace MeeShop {

class OpenReposApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MeeShop::MeeShopApplicationModel* appModel READ getAppModel NOTIFY appModelChanged)
    Q_PROPERTY(MeeShop::MeeShopCategoriesModel* categoryModel READ getCategoryModel NOTIFY categoryModelChanged)
    Q_PROPERTY(QVariantHash appInfo READ getAppInfo NOTIFY appInfoChanged)
public:
    explicit OpenReposApi(QObject *parent = nullptr) : QObject{parent},
        lastPage(0),
        appModel(new MeeShop::MeeShopApplicationModel(this)), categoryModel(new MeeShop::MeeShopCategoriesModel),
        baseUrl("http://openrepos.wunderwungiel.pl/api/v1")
    {
        request.setRawHeader("Accept-Langueage", "en");
        request.setRawHeader("Warehouse-Platform", "Harmattan");
        //QSslConfiguration config = request.sslConfiguration();
        //config.setPeerVerifyMode(QSslSocket::VerifyNone);
        //request.setSslConfiguration(config);
    }

    MeeShop::MeeShopApplicationModel* getAppModel() {return appModel;}
    MeeShop::MeeShopCategoriesModel* getCategoryModel() {return categoryModel;}
    QVariantHash getAppInfo() {return appInfo;}


    Q_INVOKABLE void getCategories();
    Q_INVOKABLE void getCategoryApps(int cat_id, int page);
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void getApplication(int app_id);
    Q_INVOKABLE void getAppComments(int app_id);
signals:
    void finished(bool sucsess);
    void appModelChanged();
    void categoryModelChanged();
    void appInfoChanged();
private:
    nlohmann::json parseJson(const QByteArray& data);
    QVariant jsonToVariant(const nlohmann::json& json);

    int lastPage;
    int currentPage;

    QString baseUrl;
    QNetworkAccessManager manager;
    QNetworkRequest request;

    MeeShop::MeeShopApplicationModel* appModel;
    MeeShop::MeeShopCategoriesModel* categoryModel;
    QVariantHash appInfo;
};

} // namespace MeeShop

#endif // MEESHOP_OPENREPOSAPI_H

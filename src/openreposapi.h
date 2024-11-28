#ifndef MEESHOP_OPENREPOSAPI_H
#define MEESHOP_OPENREPOSAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <nlohmann/json.hpp>
#include "MeeShopApplicationModel.h"
#include "MeeShopCategoriesModel.h"
#include "iostream"

namespace MeeShop {

class OpenReposApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MeeShop::MeeShopApplicationModel* appModel READ getModel NOTIFY modelChanged)
    Q_PROPERTY(MeeShop::MeeShopCategoriesModel* categoryModel READ getCatModel NOTIFY catModelChanged)
public:
    explicit OpenReposApi(QObject *parent = nullptr) : QObject{parent}, appModel(new MeeShop::MeeShopApplicationModel(this)), categoryModel(new MeeShop::MeeShopCategoriesModel(this)), baseUrl("http://openrepos.wunderwungiel.pl/api/v1") {
        request.setRawHeader("Accept-Langueage", "en");
        request.setRawHeader("Warehouse-Platform", "Harmattan");
        //QSslConfiguration config = request.sslConfiguration();
        //config.setPeerVerifyMode(QSslSocket::VerifyNone);
        //request.setSslConfiguration(config);
    }

    MeeShop::MeeShopApplicationModel* getModel() {return appModel;}
    MeeShop::MeeShopCategoriesModel* getCatModel() {return categoryModel;}

    Q_INVOKABLE void getCategories();
    Q_INVOKABLE void getCategoryApps(int cat_id, int page);
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void getAppInfo(int app_id);
    Q_INVOKABLE void getAppComments(int app_id);

private slots:
    void process_reply();
signals:
    void finished(bool sucsess);
    void modelChanged();
    void catModelChanged();
private:
    QString baseUrl;
    QString currentRoute;
    QNetworkAccessManager manager;
    QNetworkRequest request;
    QScopedPointer<QNetworkReply> reply;
    MeeShop::MeeShopApplicationModel* appModel;
    MeeShop::MeeShopCategoriesModel* categoryModel;
};

} // namespace MeeShop

#endif // MEESHOP_OPENREPOSAPI_H

#ifndef MEESHOP_OPENREPOSAPI_H
#define MEESHOP_OPENREPOSAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <nlohmann/json.hpp>
#include "MeeShopApplicationModel.h"
#include "MeeShopCategoriesModel.h"
#include "applicationinfo.h"

namespace MeeShop {

class OpenReposApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MeeShop::MeeShopApplicationModel* appModel READ getAppModel NOTIFY modelChanged)
    Q_PROPERTY(MeeShop::MeeShopCategoriesModel* categoryModel READ getCatModel NOTIFY catModelChanged)
    Q_PROPERTY(MeeShop::ApplicationInfo* appInfo READ getAppInfo NOTIFY appInfoChanged)
public:
    explicit OpenReposApi(QObject *parent = nullptr) : QObject{parent},
        lastPage(0),
        appModel(new MeeShop::MeeShopApplicationModel(this)), categoryModel(new MeeShop::MeeShopCategoriesModel(this)), appInfo(new MeeShop::ApplicationInfo(this)),
        baseUrl("http://openrepos.wunderwungiel.pl/api/v1")
    {
        request.setRawHeader("Accept-Langueage", "en");
        request.setRawHeader("Warehouse-Platform", "Harmattan");
        //QSslConfiguration config = request.sslConfiguration();
        //config.setPeerVerifyMode(QSslSocket::VerifyNone);
        //request.setSslConfiguration(config);
    }

    MeeShop::MeeShopApplicationModel* getAppModel() {return appModel;}
    MeeShop::MeeShopCategoriesModel* getCatModel() {return categoryModel;}
    MeeShop::ApplicationInfo* getAppInfo() {return appInfo;}


    Q_INVOKABLE void getCategories();
    Q_INVOKABLE void getCategoryApps(int cat_id, int page);
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void fetchAppInfo(int app_id);
    Q_INVOKABLE void getAppComments(int app_id);

private slots:
    void process_categories();
    void process_apps();
signals:
    void finished(bool sucsess);
    void modelChanged();
    void catModelChanged();
    void appInfoChanged();
private:
    nlohmann::json parseJson(QByteArray data);

    int lastPage;
    int currentPage;

    QString baseUrl;
    QNetworkAccessManager manager;
    QNetworkRequest request;

    MeeShop::MeeShopApplicationModel* appModel;
    MeeShop::MeeShopCategoriesModel* categoryModel;
    MeeShop::ApplicationInfo* appInfo;
};

} // namespace MeeShop

#endif // MEESHOP_OPENREPOSAPI_H

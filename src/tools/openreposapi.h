#ifndef MEESHOP_OPENREPOSAPI_H
#define MEESHOP_OPENREPOSAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QCoreApplication>
#include <nlohmann/json.hpp>
#include "../models/applicationmodel.h"
#include "../models/categoriesmodel.h"
#include <QVariantHash>

namespace MeeShop {

class OpenReposApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MeeShop::ApplicationModel* appModel READ getAppModel NOTIFY appModelChanged)
    Q_PROPERTY(MeeShop::CategoriesModel* categoryModel READ getCategoryModel NOTIFY categoryModelChanged)
    Q_PROPERTY(QVariantMap appInfo READ getAppInfo NOTIFY appInfoChanged)
public:
    explicit OpenReposApi(QObject *parent = nullptr) : QObject{parent},
        lastPage(0),
        appModel(new MeeShop::ApplicationModel(this)), categoryModel(new MeeShop::CategoriesModel),
        baseUrl("http://openrepos.wunderwungiel.pl/api/v1")
    {
        request.setRawHeader("Accept-Langueage", "en");
        request.setRawHeader("Warehouse-Platform", "Harmattan");
        //QSslConfiguration config = request.sslConfiguration();
        //config.setPeerVerifyMode(QSslSocket::VerifyNone);
        //request.setSslConfiguration(config);
    }

    MeeShop::ApplicationModel* getAppModel() {return appModel;}
    MeeShop::CategoriesModel* getCategoryModel() {return categoryModel;}
    QVariantMap getAppInfo() {return appInfo;}


    Q_INVOKABLE void getCategories();
    Q_INVOKABLE void getCategoryApps(int cat_id);
    Q_INVOKABLE void getCategoryAppsPage(int cat_id, int page);
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void getApplication(int app_id);
    Q_INVOKABLE void getAppComments(int app_id);

public slots:
    void process_apps();
    void process_apps_page();
    void process_categories();
    void process_app();
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

    MeeShop::ApplicationModel* appModel;
    MeeShop::CategoriesModel* categoryModel;
    QVariantMap appInfo;
};

} // namespace MeeShop

#endif // MEESHOP_OPENREPOSAPI_H

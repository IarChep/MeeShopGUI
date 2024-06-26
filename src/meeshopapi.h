#ifndef MEESHOPAPI_H
#define MEESHOPAPI_H

#include <QObject>
#include <QString>
#include <QEventLoop>
#include <QByteArray>
#include <nlohmann/json.hpp>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include "MeeShopApplicationModel.h"
#include "MeeShopCategoriesModel.h"
#include <string>

namespace MeeShop {
class MeeShopApi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MeeShop::MeeShopApplicationModel* appModel READ getModel NOTIFY modelChanged)
    Q_PROPERTY(MeeShop::MeeShopCategoriesModel* categoryModel READ getCatModel NOTIFY catModelChanged)
public:
    explicit MeeShopApi(QObject *parent = nullptr);

    MeeShop::MeeShopApplicationModel* getModel() {return appModel;}
    MeeShop::MeeShopCategoriesModel* getCatModel() {return categoryModel;}

public slots:
    void getCategories();
    void getCategoryContent(int catId, int pageNum, QString sortBy);
    int getPages(int catId);
    void getTop();
    void search(QString query, int catId, int pageNum);
    void addDownload(QString packageName);

signals:
    void finished(bool sucsess);
    void modelChanged();
    void catModelChanged();
private slots:
    void processReply();
private:
    QNetworkAccessManager manager;
    QScopedPointer<QNetworkReply> reply;
    MeeShop::MeeShopApplicationModel* appModel;
    MeeShop::MeeShopCategoriesModel* categoryModel;
    QEventLoop loop;
    bool categories;
};
}

#endif // MEESHOPAPI_H

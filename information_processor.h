#ifndef INFORMATION_PROCESSOR_H
#define INFORMATION_PROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QScopedPointer>
#include <QXmlStreamReader>
#include "info_storage.h"
#include "MeeShopApplicationModel.h"
#include <QDebug>
#include <QEventLoop>

namespace MeeShop {
    class information_processor : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(MeeShop::MeeShopApplicationModel* model READ get_final_list NOTIFY model_changed)
    public:
        explicit information_processor(QObject *parent = 0) : QObject(parent), final_model(new MeeShop::MeeShopApplicationModel(this)) {}

        Q_INVOKABLE void load_applications(QString xml);
        Q_INVOKABLE void load_rss_feeds(QString xml);

        MeeShop::MeeShopApplicationModel* get_final_list() {return final_model;}
    private slots:
        void parse_xml(QNetworkReply* reply);
        void process_reply();
        void finilase();
    signals:
        void information_loading_finished();
        void model_changed();

        void loading_error(QString error);
        void parsing_error(QString error);
    private:
        QNetworkAccessManager manager;
        QScopedPointer<QNetworkReply> reply;
        MeeShop::MeeShopApplicationModel* final_model;
        QXmlStreamReader xml_reader;
    };
}

#endif // INFORMATION_PROCESSOR_H

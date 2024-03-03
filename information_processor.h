#ifndef INFORMATION_PROCESSOR_H
#define INFORMATION_PROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QXmlStreamReader>
#include "info_storage.h"
#include <QList>
#include <algorithm>
#include <QDebug>

namespace MeeShop {
    class information_processor : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QList<QSharedPointer<MeeShop::info_storage> > ready_list READ get_final_list)
        Q_PROPERTY(QList<int> indices READ get_indices)
    public:
        explicit information_processor(QObject *parent = 0): QObject(parent){};

        Q_INVOKABLE void load_applications(QString xml);
        Q_INVOKABLE void load_rss_feeds(QString xml);

        QList<QSharedPointer<MeeShop::info_storage> > get_final_list() const {return final_list;}
        QList<int> get_indices() const {return indices;}
    private slots:
        void parse_xml(QNetworkReply* reply);
        QList<int> get_subgroup_indices(QList<QSharedPointer<info_storage> >& list);
        void process_reply();

    signals:
        void information_loading_finished();

        void loading_error(QString error);
        void parsing_error(QString error);
    private:
        QNetworkAccessManager manager;
        QScopedPointer<QNetworkReply> reply;
        QList<QSharedPointer<MeeShop::info_storage> > final_list;
        QList<int> indices;
        QXmlStreamReader xml_reader;
    };
}


#endif // INFORMATION_PROCESSOR_H

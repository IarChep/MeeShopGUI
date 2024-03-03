#include "information_processor.h"

struct CompareInfoStorage {
    bool operator()(const QSharedPointer<MeeShop::info_storage>& a, const QSharedPointer<MeeShop::info_storage>& b) {
        if (!a->app_name.isEmpty()) {
            return a->app_name < b->app_name;
        }
        else if (!a->rss_country_name.isEmpty()) {
            return a->rss_country_name < b->rss_country_name;
        }
        else if (!a->rss_feed_name.isEmpty()) {
            return a->rss_feed_name < b->rss_feed_name;
        }
        else {
            return false;
        }
    }
};



void MeeShop::information_processor::load_applications(QString xml) {
    QNetworkRequest request(QUrl("http://wunderwungiel.pl/MeeGo/openrepos/" + xml));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void MeeShop::information_processor::load_rss_feeds(QString xml) {
    QNetworkRequest request(QUrl("http://wunderwungiel.pl/MeeGo/.database/.rss/" + xml));
    reply.reset(manager.get(request));

    QObject::connect(reply.data(), SIGNAL(finished()), this, SLOT(process_reply()));
}

void MeeShop::information_processor::parse_xml(QNetworkReply *reply) {
    xml_reader.addData(reply->readAll());
    while(!xml_reader.atEnd() && !xml_reader.hasError()) {
        if(xml_reader.isStartElement() && xml_reader.name() == "app") {
            while(!(xml_reader.isEndElement() && xml_reader.name() == "app")) {
                xml_reader.readNext();
                if(xml_reader.isStartElement() && xml_reader.name() == "data") {
                    QSharedPointer<MeeShop::info_storage> is_p(new info_storage);
                    is_p->app_name = xml_reader.attributes().value("name").toString();
                    is_p->app_size = xml_reader.attributes().value("size").toString();
                    is_p->app_ver = xml_reader.attributes().value("ver").toString();;
                    is_p->app_pkg_name = xml_reader.attributes().value("package").toString();
                    if(!xml_reader.attributes().value("icon").toString().isEmpty()) {
                        is_p->app_icon = xml_reader.attributes().value("icon").toString();
                    }
                    final_list.push_back(is_p);
                }
            }
        }
        else if(xml_reader.isStartElement() && xml_reader.name() == "country") {
            QSharedPointer<MeeShop::info_storage> is_p(new info_storage);
            is_p->rss_country_name =xml_reader.attributes().value("name").toString();
            is_p->rss_country_file = xml_reader.attributes().value("file").toString();
            final_list.push_back(is_p);
        }
        else if (xml_reader.isStartElement() && xml_reader.name() == "rss") {
            QSharedPointer<MeeShop::info_storage> is_p(new info_storage);;
            is_p->rss_feed_name = xml_reader.attributes().value("name").toString();
            is_p->rss_feed_url =xml_reader.attributes().value("url").toString();
            final_list.push_back(is_p);
        }
        xml_reader.readNext();
    }
    if (xml_reader.hasError()){
        emit parsing_error(xml_reader.errorString());
    }
    else {
        indices = get_subgroup_indices(final_list);
    }
}

void MeeShop::information_processor::process_reply() {
    if (reply->error() == QNetworkReply::NoError) {
        parse_xml(reply.data());
    } else {
        emit loading_error(reply->errorString());
    }
}

QList<int> MeeShop::information_processor::get_subgroup_indices(QList<QSharedPointer<info_storage> >& list) {
    std::sort(list.begin(), list.end(), CompareInfoStorage());
    QList<int> indices;
    QChar current_char = '\0';
    for (ushort c = '0'; c <= 'z'; c++) {
        QChar ch = QChar(c);
        QSharedPointer<info_storage> temp(new info_storage);
        temp->app_name = QString(ch);
        QList<QSharedPointer<info_storage> >::iterator it = std::lower_bound(list.begin(), list.end(), temp, CompareInfoStorage());
        if (it != list.end() && (**it).app_name[0] == ch && ch != current_char) {
            indices.push_back(it - list.begin());
            current_char = ch;
        }
    }
    qDebug() << "Subgroup finished";
    emit information_loading_finished();
    foreach(int i, indices) {
        qDebug() << "Indices: " << i;
    }

    return indices;
}

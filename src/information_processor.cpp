#include "information_processor.h"



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
void MeeShop::information_processor::process_reply() {
    if (reply->error() == QNetworkReply::NoError) {
        parse_xml(reply.data());
    } else {
        emit loading_error(reply->errorString());
    }

}
void MeeShop::information_processor::parse_xml(QNetworkReply *reply) {
    xml_reader.addData(reply->readAll());
    while(!xml_reader.atEnd() && !xml_reader.hasError()) {
        if(xml_reader.isStartElement() && xml_reader.name() == "app") {
            while(!(xml_reader.isEndElement() && xml_reader.name() == "app")) {
                xml_reader.readNext();
                if(xml_reader.isStartElement() && xml_reader.name() == "data") {
                    MeeShop::info_storage is;
                    is.app_name = xml_reader.attributes().value("name").toString();
                    is.letter = is.app_name[0].toUpper();
                    is.app_size = xml_reader.attributes().value("size").toString();
                    is.app_ver = xml_reader.attributes().value("ver").toString();;
                    is.app_pkg_name = xml_reader.attributes().value("package").toString();
                    if(!xml_reader.attributes().value("icon").toString().isEmpty()) {
                        is.app_icon = xml_reader.attributes().value("icon").toString();
                    }
                    final_model->addEntry(is);
                }
            }
        }
        else if(xml_reader.isStartElement() && xml_reader.name() == "country") {
            MeeShop::info_storage is;
            is.rss_country_name =xml_reader.attributes().value("name").toString();
            is.letter = is.rss_country_name[0].toUpper();
            is.rss_country_file = xml_reader.attributes().value("file").toString();
            final_model->addEntry(is);
        }
        else if (xml_reader.isStartElement() && xml_reader.name() == "rss") {
            MeeShop::info_storage is;
            is.rss_feed_name = xml_reader.attributes().value("name").toString();
            is.letter = is.rss_feed_name[0].toUpper();
            is.rss_feed_url =xml_reader.attributes().value("url").toString();
            final_model->addEntry(is);
        }
        xml_reader.readNext();
    }
    if (xml_reader.hasError()){
        emit parsing_error(xml_reader.errorString());
    }
    else {
        QObject::connect(final_model, SIGNAL(sorting_finished()), SLOT(finilase()));
        final_model->sort();
    }
}
void MeeShop::information_processor::finilase() {
    emit model_changed();
    emit information_loading_finished();

}



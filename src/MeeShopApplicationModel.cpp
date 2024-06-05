#include "MeeShopApplicationModel.h"

MeeShop::MeeShopApplicationModel::MeeShopApplicationModel(QObject *parent)
    : QAbstractListModel(parent)
{    
    QHash<int, QByteArray> roles;
    roles[AppNameRole] = "app_name";
    roles[AppVerRole] = "app_ver";
    roles[AppSizeRole] = "app_size";
    roles[AppPkgNameRole] = "app_pkg_name";
    roles[AppIconRole] = "app_icon";

    roles[RssCountryNameRole] = "rss_country_name";
    roles[RssCountryFileRole] = "rss_country_file";

    roles[RssFeedNameRole] = "rss_feed_name";
    roles[RssFeedUrlRole] = "rss_feed_url";

    roles[LetterRole] = "letter";
    setRoleNames(roles);
}


void MeeShop::MeeShopApplicationModel::addEntry(const MeeShop::info_storage &is)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_entries << is;
    endInsertRows();
}

int MeeShop::MeeShopApplicationModel::rowCount(const QModelIndex &parent) const {
    return m_entries.count();
}

QVariant MeeShop::MeeShopApplicationModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() > m_entries.count())
        return QVariant();

    const MeeShop::info_storage &is = m_entries[index.row()];

    switch (role) {
    case AppNameRole:
        return is.app_name;
    case AppVerRole:
        return is.app_ver;
    case AppSizeRole:
        return is.app_size;
    case AppPkgNameRole:
        return is.app_pkg_name;
    case AppIconRole:
        return is.app_icon;

    case RssCountryNameRole:
        return is.rss_country_name;
    case RssCountryFileRole:
        return is.rss_country_file;

    case RssFeedNameRole:
        return is.rss_feed_name;
    case RssFeedUrlRole:
        return is.rss_feed_url;

    case LetterRole:
        return is.letter;
    }
    return QVariant();
}
void MeeShop::MeeShopApplicationModel::slice(int from, int to) {
    if (!recover_entries.empty()) {
        m_entries = recover_entries.mid(from, to-from+1);
    } else {
        recover_entries = m_entries;
        m_entries = m_entries.mid(from, to-from+1);
    }
}

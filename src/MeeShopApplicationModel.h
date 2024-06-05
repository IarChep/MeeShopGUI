#ifndef MEESHOPMODEL_H
#define MEESHOPMODEL_H
#include <QAbstractListModel>
#include "info_storage.h"
#include <QList>
#include <algorithm>
#include <QDebug>

namespace MeeShop {
class MeeShopApplicationModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum EntryRoles {
        AppNameRole = Qt::UserRole + 1,
        AppVerRole,
        AppSizeRole,
        AppPkgNameRole,
        AppIconRole,

        RssCountryNameRole,
        RssCountryFileRole,

        RssFeedNameRole,
        RssFeedUrlRole,

        LetterRole
    };
    struct Compare {
        bool operator()(const MeeShop::info_storage& a, const MeeShop::info_storage& b) const {
            if (!a.app_name.isEmpty() && !b.app_name.isEmpty()) {
                return a.app_name.compare(b.app_name, Qt::CaseInsensitive) < 0;
            }
            else if (!a.rss_country_name.isEmpty() && b.rss_country_name.isEmpty()) {
                return a.rss_country_name.compare(b.rss_country_name, Qt::CaseInsensitive) < 0;
            } else {
                return a.rss_feed_name.compare(b.rss_feed_name, Qt::CaseInsensitive) < 0;
            }
        }
    };

    MeeShopApplicationModel(QObject *parent = 0);

    void addEntry(const MeeShop::info_storage &is);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void sort() {
        std::sort(m_entries.begin(), m_entries.end(), Compare());
        emit sorting_finished();
    }

    Q_INVOKABLE void slice(int from, int to);
signals:
    void sorting_finished();
private:
    QList<MeeShop::info_storage> m_entries;
    QList<MeeShop::info_storage> recover_entries;
};
}

Q_DECLARE_METATYPE(MeeShop::MeeShopApplicationModel*)
#endif // MEESHOPMODEL_H



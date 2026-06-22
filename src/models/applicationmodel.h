#ifndef MEESHOPAPPSMODEL_H
#define MEESHOPAPPSMODEL_H

#include <QAbstractListModel>
#include <nlohmann/json.hpp>
#include <QList>
#include <algorithm>
#include <QDebug>

using json = nlohmann::json;

namespace MeeShop {
class ApplicationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    int count() const { return rowCount(); }
    enum EntryRoles {
        AppNameRole = Qt::UserRole + 1,
        AppVerRole,
        AppDevRole,
        AppIdRole,
        AppIconRole,
        AppRatingRole,       // 0..100 (rating.rating)
        AppRatingCountRole,  // число оценок (rating.count)
        AppCommentsRole,     // число комментариев (comments_count)
    };

    explicit ApplicationModel(QObject *parent = 0) : QAbstractListModel(parent)
    {
        QHash<int, QByteArray> roles;
        roles[AppNameRole] = "appName";
        roles[AppVerRole] = "appVer";
        roles[AppDevRole] = "appDev";
        roles[AppIdRole] = "appId";
        roles[AppIconRole] = "appIcon";
        roles[AppRatingRole] = "appRating";
        roles[AppRatingCountRole] = "appRatingCount";
        roles[AppCommentsRole] = "appComments";
        setRoleNames(roles);
    }

    // Максимум страниц, одновременно хранимых в модели (скользящее окно).
    static const int MaxPages = 3;

    void pushPageBack(const json &page);
    void pushPageFront(const json &page);
    void reset();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE bool isEmpty() {
        return m_jsonList.empty();
    }

signals:
    void pageBackAdded();
    void pageFrontAdded(int frontAddedSize);
    void countChanged();

private:
    QList<json> m_jsonList;
};
}

Q_DECLARE_METATYPE(MeeShop::ApplicationModel*)
#endif // MEESHOPMODEL_H



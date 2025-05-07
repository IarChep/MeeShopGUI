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
public:
    enum EntryRoles {
        AppNameRole = Qt::UserRole + 1,
        AppVerRole,
        AppDevRole,
        AppIdRole,
        AppIconRole,
    };

    explicit ApplicationModel(QObject *parent = 0) : QAbstractListModel(parent)
    {
        QHash<int, QByteArray> roles;
        roles[AppNameRole] = "appName";
        roles[AppVerRole] = "appVer";
        roles[AppDevRole] = "appDev";
        roles[AppIdRole] = "appId";
        roles[AppIconRole] = "appIcon";
        setRoleNames(roles);
    }

    void pushPageBack(const json &jsonDoc);
    void pushPageFront(const json &jsonDoc);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

signals:
    void pageBackAdded(int frontDeletedSize);
    void pageFrontAdded(int frontAddedSize);
private:
    QList<json> m_jsonList;
};
}

Q_DECLARE_METATYPE(MeeShop::ApplicationModel*)
#endif // MEESHOPMODEL_H



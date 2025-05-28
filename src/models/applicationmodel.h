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

    void setCachePage(const json &jsonDoc);
    void pushPageBack(const json &jsonDoc);
    void pushPageFront(const json &jsonDoc);
    void nextPageFromCache();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

<<<<<<< HEAD
=======
    Q_INVOKABLE bool isEmpty() {
        return m_jsonList.empty() && m_cachedPage.empty();
    }

>>>>>>> 982f1f7 (Almost finished app list functionality. Categories now fully work)
signals:
    void pageBackAdded(int frontDeletedSize);
    void pageFrontAdded(int frontAddedSize);

private:
    QList<json> m_jsonList;
    json m_cachedPage;
};
}

Q_DECLARE_METATYPE(MeeShop::ApplicationModel*)
#endif // MEESHOPMODEL_H



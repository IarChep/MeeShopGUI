#ifndef MEESHOPAPPSMODEL_H
#define MEESHOPAPPSMODEL_H
#include <QAbstractListModel>
#include <nlohmann/json.hpp>
#include <QList>
#include <algorithm>
#include <QDebug>

using json = nlohmann::json;

namespace MeeShop {
class MeeShopApplicationModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum EntryRoles {
        AppNameRole = Qt::UserRole + 1,
        AppVerRole,
        AppSizeRole,
        AppDevRole,
        AppPkgNameRole,
        AppIconRole,
        LetterRole,
        DevLetterRole,
    };

    MeeShopApplicationModel(QObject *parent = 0);

    void setJson(const json &jsonDoc);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
    json m_json;
};
}

Q_DECLARE_METATYPE(MeeShop::MeeShopApplicationModel*)
#endif // MEESHOPMODEL_H



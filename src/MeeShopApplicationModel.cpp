#include "MeeShopApplicationModel.h"

MeeShop::MeeShopApplicationModel::MeeShopApplicationModel(QObject *parent)
    : QAbstractListModel(parent)
{    
    QHash<int, QByteArray> roles;
    roles[AppNameRole] = "appName";
    roles[AppVerRole] = "appVer";
    roles[AppDevRole] = "appDev";
    roles[AppIdRole] = "appId";
    roles[AppIconRole] = "appIcon";
    setRoleNames(roles);
}


void MeeShop::MeeShopApplicationModel::setJson(const json &jsonDoc)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_json = jsonDoc;
    qDebug() << QString::fromStdString(jsonDoc.dump(4)) << jsonDoc.size();
    endInsertRows();
}
int MeeShop::MeeShopApplicationModel::rowCount(const QModelIndex &parent) const {
    return m_json.size();
}

QVariant MeeShop::MeeShopApplicationModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() > m_json.size())
        return QVariant();

    const json &jsonElem = m_json.at(index.row());

    switch (role) {
    case AppNameRole:
        if (jsonElem.contains("title"))
            return QString::fromStdString(jsonElem["title"].get<std::string>());
        break;
    case AppVerRole:
        if (jsonElem.contains("version"))
            return QString::fromStdString(jsonElem["version"].get<std::string>());
    case AppDevRole:
        if (jsonElem.contains("user"))
            return QString::fromStdString(jsonElem["user"]["name"].get<std::string>());
        break;
    case AppIdRole:
        if (jsonElem.contains("appid"))
            return jsonElem["appid"].get<int>();
        break;
    case AppIconRole:
        if (jsonElem.contains("icon")) {
            return QString::fromStdString(jsonElem["icon"]["url"].get<std::string>());
        }
        break;
    }
    return QVariant();
}


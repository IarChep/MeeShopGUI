#include "MeeShopApplicationModel.h"

MeeShop::MeeShopApplicationModel::MeeShopApplicationModel(QObject *parent)
    : QAbstractListModel(parent)
{    
    QHash<int, QByteArray> roles;
    roles[AppNameRole] = "appName";
    roles[AppVerRole] = "appVer";
    roles[AppDevRole] = "appDev";
    roles[AppSizeRole] = "appSize";
    roles[AppPkgNameRole] = "appPkgName";
    roles[AppIconRole] = "appIcon";
    roles[LetterRole] = "letter";
    roles[DevLetterRole] = "devLetter";
    setRoleNames(roles);
}


void MeeShop::MeeShopApplicationModel::setJson(const json &jsonDoc)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_json = jsonDoc;
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
        return QString::fromStdString(jsonElem["title"].get<std::string>());
    case AppVerRole:
        return QString::fromStdString(jsonElem["version"].get<std::string>());
    case AppSizeRole:
        return jsonElem["size"].get<int>() / 1024;
    case AppDevRole:
        return QString::fromStdString(jsonElem["publisher"].get<std::string>());
    case AppPkgNameRole:
        return QString::fromStdString(jsonElem["package"].get<std::string>());
    case AppIconRole:
        return QString::fromStdString(jsonElem["img"].get<std::string>());
    case LetterRole:
        return QString(toupper(jsonElem["title"].get<std::string>()[0]));
    case DevLetterRole:
        return QString(toupper(jsonElem["publisher"].get<std::string>()[0]));

    return QVariant();
    }
}


#include "MeeShopCategoriesModel.h"

MeeShop::MeeShopCategoriesModel::MeeShopCategoriesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[CategoryAmountRole] = "categoryAmount";
    roles[CategoryDirRole] = "categoryDir";
    roles[CategoryIdRole] = "categoryId";
    roles[CategoryNameRole] = "categoryName";
    setRoleNames(roles);
}


void MeeShop::MeeShopCategoriesModel::setJson(const json &jsonDoc)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_json = jsonDoc;
    endInsertRows();
    emit countChanged();
}
int MeeShop::MeeShopCategoriesModel::rowCount(const QModelIndex &parent) const {
    return m_json.size();
}

QVariant MeeShop::MeeShopCategoriesModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() > m_json.size())
        return QVariant();

    const json &jsonElem = m_json.at(index.row());

    switch (role) {
    case CategoryAmountRole:
        return jsonElem["amount"].get<int>();
    case CategoryDirRole:
        return QString::fromStdString(jsonElem["directory"].get<std::string>());
    case CategoryIdRole:
        return jsonElem["id"].get<int>();
    case CategoryNameRole:
        return QString::fromStdString(jsonElem["name"].get<std::string>());

    }
    return QVariant();
}


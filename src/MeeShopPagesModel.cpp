#include "MeeShopPagesModel.h"

MeeShop::MeeShopPagesModel::MeeShopPagesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[PagesRole] = "page";
    setRoleNames(roles);
}


void MeeShop::MeeShopPagesModel::setPages(const int &pages)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_pages = pages;
    endInsertRows();
    emit countChanged();
}
int MeeShop::MeeShopPagesModel::rowCount(const QModelIndex &parent) const {
    return m_pages;
}

QVariant MeeShop::MeeShopPagesModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() > m_pages)
        return QVariant();

    switch (role) {
    case PagesRole:
        return index.row() + 1;
    }
    return QVariant();
}


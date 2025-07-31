#include "installationqueuemodel.h"

namespace MeeShop {

int InstallationQueueModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_queue.size();
}

QVariant InstallationQueueModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    int row = index.row();
    switch (role) {
    case AppNameRole:
        return m_queue[row].first.name;
        break;
    case AppPackageRole:
        return m_queue[row].first.package;
        break;
    case AppPublisherRole:
        return m_queue[row].first.publisher;
        break;
    default:
        return QVariant();
        break;
    }
}

} // namespace MeeShop

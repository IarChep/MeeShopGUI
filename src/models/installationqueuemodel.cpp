#include "installationqueuemodel.h"

#include <QVariant>

namespace MeeShop {

// Значения должны совпадать с InstallationQueue::TaskStatus:
// Queued = 0, Running = 1, Done = 2, Failed = 3.
bool InstallationQueueModel::isActive(int status) {
    return status == 0 || status == 1;
}

InstallationQueueModel::InstallationQueueModel(QObject *parent)
    : QAbstractListModel(parent) {
    QHash<int, QByteArray> roles;
    roles[AppIdRole]         = "appId";
    roles[NameRole]          = "name";
    roles[DeveloperRole]     = "developer";
    roles[IconUrlRole]       = "iconUrl";
    roles[OperationRole]     = "operation";
    roles[StatusRole]        = "status";
    roles[ActionRole]        = "action";
    roles[ProgressRole]      = "progress";
    roles[IndeterminateRole] = "indeterminate";
    roles[LogRole]           = "logModel";
    setRoleNames(roles);
}

InstallationQueueModel::~InstallationQueueModel() {
    qDeleteAll(m_entries);
    m_entries.clear();
}

int InstallationQueueModel::activeCount() const {
    int n = 0;
    for (int i = 0; i < m_entries.size(); ++i)
        if (isActive(m_entries.at(i)->status))
            ++n;
    return n;
}

int InstallationQueueModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : m_entries.size();
}

QVariant InstallationQueueModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
        return QVariant();
    const Entry *e = m_entries.at(index.row());
    switch (role) {
    case AppIdRole:         return e->appId;
    case NameRole:          return e->name;
    case DeveloperRole:     return e->developer;
    case IconUrlRole:       return e->iconUrl;
    case OperationRole:     return e->operation;
    case StatusRole:        return e->status;
    case ActionRole:        return e->action;
    case ProgressRole:      return e->progress;
    case IndeterminateRole: return e->indeterminate;
    case LogRole:           return QVariant::fromValue<QObject*>(e->log);
    }
    return QVariant();
}

int InstallationQueueModel::indexOfAppId(int appId) const {
    for (int i = 0; i < m_entries.size(); ++i)
        if (m_entries.at(i)->appId == appId)
            return i;
    return -1;
}

InstallationQueueModel::Entry *InstallationQueueModel::at(int row) const {
    if (row < 0 || row >= m_entries.size())
        return 0;
    return m_entries.at(row);
}

InstallationQueueModel::Entry *InstallationQueueModel::byAppId(int appId) const {
    const int row = indexOfAppId(appId);
    return row >= 0 ? m_entries.at(row) : 0;
}

int InstallationQueueModel::addEntry(int appId, const QString &name, const QString &developer,
                                     const QString &iconUrl, const QString &packageName, int operation) {
    Entry *e = new Entry;
    e->appId = appId;
    e->name = name;
    e->developer = developer;
    e->iconUrl = iconUrl;
    e->packageName = packageName;
    e->operation = operation;
    e->status = 0;            // Queued
    e->action = QString();
    e->progress = 0;
    e->indeterminate = true;
    e->log = new AptLogModel(this);

    const int row = m_entries.size();
    beginInsertRows(QModelIndex(), row, row);
    m_entries.append(e);
    endInsertRows();
    emit countChanged();
    emit activeCountChanged(); // новая запись активна (Queued)
    return row;
}

void InstallationQueueModel::removeRow(int row) {
    if (row < 0 || row >= m_entries.size())
        return;
    const bool wasActive = isActive(m_entries.at(row)->status);
    beginRemoveRows(QModelIndex(), row, row);
    Entry *e = m_entries.takeAt(row);
    endRemoveRows();
    // deleteLater, а не delete: открытый AptLogSheet может ещё держать этот лог как
    // model своего ListView. Отложенное удаление даёт QML обнулить ссылку
    // (logModel — guarded QtObject) до фактического разрушения объекта.
    e->log->deleteLater();
    delete e;
    emit countChanged();
    if (wasActive)
        emit activeCountChanged();
}

void InstallationQueueModel::moveRow(int from, int to) {
    const int n = m_entries.size();
    if (from < 0 || from >= n || to < 0 || to >= n || from == to)
        return;
    const int dest = (to > from) ? to + 1 : to;
    beginMoveRows(QModelIndex(), from, from, QModelIndex(), dest);
    m_entries.move(from, to);
    endMoveRows();
}

void InstallationQueueModel::setStatus(int row, int status) {
    Entry *e = at(row);
    if (!e || e->status == status)
        return;
    const bool was = isActive(e->status);
    e->status = status;
    emitChanged(row);
    if (was != isActive(status))
        emit activeCountChanged();
}

void InstallationQueueModel::setAction(int row, const QString &action, bool indeterminate) {
    Entry *e = at(row);
    if (!e)
        return;
    e->action = action;
    e->indeterminate = indeterminate;
    emitChanged(row);
}

void InstallationQueueModel::setProgress(int row, int progress) {
    Entry *e = at(row);
    if (!e || e->progress == progress)
        return;
    e->progress = progress;
    emitChanged(row);
}

void InstallationQueueModel::emitChanged(int row) {
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx);
}

}

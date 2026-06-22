#ifndef INSTALLATIONQUEUEMODEL_H
#define INSTALLATIONQUEUEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QHash>
#include <QByteArray>

#include "../tools/apt/aptlogmodel.h"

namespace MeeShop {

// Список приложений в очереди установки: минимальная инфа + живой статус,
// прогресс и собственный лог apt у каждого элемента.
class InstallationQueueModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY activeCountChanged)
public:
    enum Roles {
        AppIdRole = Qt::UserRole + 1,
        NameRole,
        DeveloperRole,
        IconUrlRole,
        OperationRole,
        StatusRole,
        ActionRole,
        ProgressRole,
        IndeterminateRole,
        LogRole
    };

    struct Entry {
        int appId;
        QString name;
        QString developer;
        QString iconUrl;
        QString packageName;
        int operation;
        int status;
        QString action;
        int progress;
        bool indeterminate;
        AptLogModel *log;
    };

    explicit InstallationQueueModel(QObject *parent = 0);
    ~InstallationQueueModel();

    int count() const { return m_entries.size(); }
    int activeCount() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // --- API для контроллера (InstallationQueue) ---
    int indexOfAppId(int appId) const;
    Entry *at(int row) const;
    Entry *byAppId(int appId) const;

    int addEntry(int appId, const QString &name, const QString &developer,
                 const QString &iconUrl, const QString &packageName, int operation);
    void removeRow(int row);
    void moveRow(int from, int to);

    void setStatus(int row, int status);
    void setAction(int row, const QString &action, bool indeterminate);
    void setProgress(int row, int progress);

signals:
    void countChanged();
    void activeCountChanged();

private:
    static bool isActive(int status); // Queued или Running
    void emitChanged(int row);

    QList<Entry*> m_entries;
};

}

#endif // INSTALLATIONQUEUEMODEL_H

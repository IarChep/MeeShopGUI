#ifndef MEESHOP_INSTALLATIONQUEUEMODEL_H
#define MEESHOP_INSTALLATIONQUEUEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QString>
#include <deque>
#include <QHash>
#include <QVariantMap>

namespace MeeShop {

struct ApplicationInfo {
    QString name;
    QString package;
    QString publisher;
};

struct InstallationStatus {
    QString status = "Queued";
    int progress = -1;

    QVariantMap toMap() {
        QVariantMap map;
        map["status"] = this->status;
        map["progress"] = this->progress;
        return map;
    }
};

class InstallationQueueModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum EntryRoles {
        AppNameRole,
        AppPackageRole,
        AppPublisherRole
    };

    explicit InstallationQueueModel(QObject *parent, std::deque<std::pair<ApplicationInfo, InstallationStatus>>& queue) : QAbstractListModel(parent), m_queue(queue)
    {
        QHash<int, QByteArray> roles;
        roles[AppNameRole] = "appName";
        roles[AppPackageRole] = "appPackage";
        roles[AppPublisherRole] = "appPublisher";
        setRoleNames(roles);
    }

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

private:
    std::deque<std::pair<ApplicationInfo, InstallationStatus>>& m_queue;
};

} // namespace MeeShop
Q_DECLARE_METATYPE(MeeShop::InstallationQueueModel*)

#endif // MEESHOP_INSTALLATIONQUEUEMODEL_H

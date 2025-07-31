#ifndef MEESHOP_INSTALLATIONQUEUE_H
#define MEESHOP_INSTALLATIONQUEUE_H

#include <QObject>
#include <QVariantMap>
#include "../models/installationqueuemodel.h"
#include <deque>
#include <unordered_map>

namespace MeeShop {

class InstallationQueue : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MeeShop::InstallationQueueModel* queueModel READ getModel() NOTIFY modelChanged())
public:
    explicit InstallationQueue(QObject *parent = nullptr) : m_model(new MeeShop::InstallationQueueModel(this, m_queue)) {}

    MeeShop::InstallationQueueModel* getModel() {
        return m_model;
    }

    Q_INVOKABLE void add(QString name, QString package, QString publisher) {
        ApplicationInfo info;
        info.name = name;
        info.package = package;
        info.publisher = publisher;

        InstallationStatus status;
        status.status = "Queued";

        m_queue.emplace_back(info, status);
        m_indexMap[package.toStdString()] = m_queue.size() - 1;
    }
    Q_INVOKABLE QVariantMap getStatus(QString package) {
        if (!m_indexMap.contains(package.toStdString())) {
            return QVariantMap({{"status", "toInstall"}});
        }
        return m_queue[m_indexMap[package.toStdString()]].second.toMap();
    }
signals:
    void modelChanged();
private:
    std::unordered_map<std::string, int> m_indexMap;
    std::deque<std::pair<ApplicationInfo, InstallationStatus>> m_queue;
    MeeShop::InstallationQueueModel* m_model;

    void fixIndexes() {
        for (auto& [package, index] : m_indexMap) {
            index--;
        }
    }
};

} // namespace MeeShop

#endif // MEESHOP_INSTALLATIONQUEUE_H

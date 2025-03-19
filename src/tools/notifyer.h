#ifndef NOTIFYER_H
#define NOTIFYER_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariantMap>
#include <QDebug>

namespace MeeShop {
class Notifyer : public QObject
{
    Q_OBJECT
public:
    explicit Notifyer(QObject *parent = nullptr);

public slots:
    void sendPopup(QString body);
    void sendNotification(QString body);
private:
    QDBusConnection bus;
    QDBusInterface notificationManager;
};
}

#endif // NOTIFYER_H

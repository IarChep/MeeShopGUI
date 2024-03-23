#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <QObject>
#include <QtDBus/QDBusMessage>
namespace MeeShop {
    class notification_manager : public QObject
    {
        Q_OBJECT
    public:
        explicit notification_manager(QObject *parent = 0);

    public slots:
        void send_notification(QString text);
        void send_big_notification(QString text);
    };
}


#endif // NOTIFICATION_MANAGER_H

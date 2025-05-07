#include "notifyer.h"


namespace MeeShop {
Notifyer::Notifyer(QObject *parent)
    : QObject{parent} , bus(QDBusConnection::sessionBus()), notificationManager("com.meego.core.MNotificationManager",
                                                                                "/notificationmanager",
                                                                                "com.meego.core.MNotificationManager",
                                                                                bus)
{
    if (!bus.isConnected()) {
        qDebug() << "Failed to connect to dbus";
    }

    if (!notificationManager.isValid()) {
        qDebug() << "Failed to connect to notification managerй";
    }
}
void Notifyer::sendPopup(QString body) {
    QDBusReply<uint32_t> userIdReply = notificationManager.call("notificationUserId");
    if (!userIdReply.isValid()) {
        qDebug() << "cant get user id";
    }
    uint32_t userId = userIdReply.value();

    // Подготовка данных уведомления
    QVariantMap data;
    data["eventType"] = "x-nokia.internet";  // Тип события
    data["summary"] = "MeeShop";                   // Заголовок
    data["body"] = body;            // Текст
    data["imageId"] = "/usr/share/icons/hicolor/80x80/apps/meeshop80.png";  // Путь к изображению

    QDBusReply<uint32_t> addReply = notificationManager.call("addNotification", userId, 0U, data);
    if (!addReply.isValid()) {
        qDebug() << "Couldnt send the popup";
    }
    uint32_t notificationId = addReply.value();
    qDebug() << "Sent popup with ID:" << notificationId;
}
void Notifyer::sendNotification(QString body) {
    QDBusReply<uint32_t> userIdReply = notificationManager.call("notificationUserId");
    if (!userIdReply.isValid()) {
        qDebug() << "cant get user id";
    }
    uint32_t userId = userIdReply.value();
    qDebug() << "Notification user id:" << userId;

    // Подготовка данных уведомления
    QVariantMap data;
    data["eventType"] = "custom";
    data["summary"] = "MeeShop";
    data["body"] = body;
    data["imageId"] = "/usr/share/icons/hicolor/80x80/apps/meeshop80.png";  // Путь к изображению

    QDBusReply<uint32_t> addReply = notificationManager.call("addNotification", userId, 0U, data);
    if (!addReply.isValid()) {
        qDebug() << "Couldnt send the notification";
    }
    uint32_t notificationId = addReply.value();
    qDebug() << "Sent notification with ID:" << notificationId;
}
}

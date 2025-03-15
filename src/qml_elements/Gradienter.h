#ifndef GRADIENTER_H
#define GRADIENTER_H

#include <QImage>
#include <QColor>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QEventLoop>
#include <QDebug>
#include <QGradient>
#include <QtDeclarative/private/qdeclarativerectangle_p.h>
#include <QtDeclarative/private/qdeclarativelist_p.h>

namespace MeeShop {
class Gradienter : public QObject {
    Q_OBJECT
    Q_PROPERTY(QDeclarativeGradient* gradient READ getGradient() NOTIFY gradientChanged())
public:
    explicit Gradienter(QObject *parent = 0) : QObject(parent), m_gradient(new QDeclarativeGradient(this)) {}
    QDeclarativeGradient* getGradient() {
        return m_gradient;
    }
public slots:
    void getGradientColors(QString image_path) {
        QNetworkRequest request(image_path);
        QNetworkReply* reply = manager.get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(process_gradient()));
    }
    void process_gradient() {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        QImage image;
        image.loadFromData(reply->readAll());

        int middle = image.width() / 2;

        QDeclarativeGradientStop* stop0 = new QDeclarativeGradientStop(m_gradient);
        stop0->setPosition(0.0);
        stop0->setColor(QColor(image.pixel(middle, 5)));

        QDeclarativeGradientStop* stop1 = new QDeclarativeGradientStop(m_gradient);
        stop1->setPosition(1.0);
        stop1->setColor(QColor(image.pixel(middle, image.height() - 5)));

        auto stops = m_gradient->stops();
        stops.clear;
        stops.append(&stops, stop0);
        stops.append(&stops, stop1);
        emit gradientChanged();

        reply->deleteLater();
    }
signals:
    void gradientChanged();
private:
    QDeclarativeGradient* m_gradient;
    QNetworkAccessManager manager;
};
}


#endif // GRADIENTER_H

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

namespace MeeShop {
class Gradienter : public QObject {
    Q_OBJECT
public:
    explicit Gradienter(QObject *parent = 0) : QObject(parent) {}

public slots:
    QStringList get_gradient_colors(QString image_path) {
        QNetworkRequest request(image_path);
        reply.reset(manager.get(request));

        QEventLoop loop;
        QObject::connect(reply.data(), SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        QImage image;
        image.loadFromData(reply->readAll());

        int middle = image.width() / 2;
        QColor top_pixel = QColor(image.pixel(middle, 0));
        QColor bottom_pixel = QColor(image.pixel(middle, image.height() - 1));

        QStringList colors;
        colors.push_back(top_pixel.name());
        colors.push_back(bottom_pixel.name());

        return colors;
    }
private:
    QNetworkAccessManager manager;
    QScopedPointer<QNetworkReply> reply;
};
}


#endif // GRADIENTER_H

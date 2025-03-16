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
#include <QStringList>

namespace MeeShop {
class Gradienter : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList gradientColors READ getGradient() NOTIFY gradientChanged())
public:
    explicit Gradienter(QObject *parent = 0) : QObject(parent) {}
    QStringList getGradient() {
        return m_colors;
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

        m_colors.clear();

        m_colors.push_back(QColor(image.pixel(middle, 5)).name());
        m_colors.push_back(QColor(image.pixel(middle, image.height() - 5)).name());
        emit gradientChanged();

        reply->deleteLater();
    }
signals:
    void gradientChanged();
private:
    QStringList m_colors;
    QNetworkAccessManager manager;
};
}


#endif // GRADIENTER_H

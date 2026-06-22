#ifndef PROGRESSINDICATOR_H
#define PROGRESSINDICATOR_H

#include <QDeclarativeItem>
#include <qdeclarative.h>
#include <QPainter>
#include <QPixmap>
#include <cmath>

namespace MeeShop {
class ProgressIndicator : public QDeclarativeItem {
    Q_OBJECT
    Q_PROPERTY(QString size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
    explicit ProgressIndicator(QDeclarativeItem *parent = 0) : QDeclarativeItem(parent), m_progress(0) {
        setFlag(QGraphicsItem::ItemHasNoContents, false);
    }

    QString size() const { return m_size; }
    void setSize(const QString &size) {
        if (m_size == size) return;
        m_size = size;
        loadImages();
        emit sizeChanged();
    }

    int progress() const { return m_progress; }
    void setProgress(int progress) {
        if (progress < 0) progress = 0;
        if (progress > 100) progress = 100;
        if (m_progress == progress) return;
        m_progress = progress;
        update();
        emit progressChanged();
    }

signals:
    void sizeChanged();
    void progressChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        if (basePixmap.isNull() || maskPixmap.isNull()) return;

        // Draw the base pixmap
        painter->drawPixmap(0, 0, basePixmap);

        // Draw progress if > 0
        if (m_progress > 0) {
            qreal centerX = width() / 2.0;
            qreal centerY = height() / 2.0;
            qreal radius = floor(width() * sqrt(2) / 2) + 1;
            QRectF rect(centerX - radius, centerY - radius, 2 * radius, 2 * radius);

            QPainterPath path;
            path.moveTo(centerX, centerY);
            path.arcTo(rect, 90, -(m_progress / 100.0) * 360);
            path.closeSubpath();

            // ВАЖНО: painter в QtQuick1 (QGraphicsView/QDeclarativeItem) ОБЩИЙ для всей
            // сцены; элемент обязан восстановить любое изменённое им состояние. Раньше
            // здесь стоял setClipping(false) — он не возвращал исходный клип, а отключал
            // отсечение целиком, портя отрисовку всего, что рисуется ПОСЛЕ (тулбар и
            // системный статус-бар «пропадали», но оставались кликабельными). save()/
            // restore() гарантированно возвращают клип; IntersectClip не даёт выйти за
            // пределы текущего клипа даже на время отрисовки маски.
            painter->save();
            painter->setClipPath(path, Qt::IntersectClip);
            painter->drawPixmap(0, 0, maskPixmap);
            painter->restore();
        }
    }

private:
    void loadImages() {
        // Load images based on size
        QString baseFile, maskFile;
        int sizeValue = 24; // default tiny
        if (m_size == "tiny") {
            sizeValue = 24;
            baseFile = ":/assets/progressindicator/base-24.png";
            maskFile = ":/assets/progressindicator/mask-24.png";
        } else if (m_size == "small") {
            sizeValue = 32;
            baseFile = ":/assets/progressindicator/base-32.png";
            maskFile = ":/assets/progressindicator/mask-32.png";
        } else if (m_size == "medium") {
            sizeValue = 64;
            baseFile = ":/assets/progressindicator/base-64.png";
            maskFile = ":/assets/progressindicator/mask-64.png";
        } else if (m_size == "big" || m_size == "large") { // ExtendedIndicator зовёт 96 как "large"
            sizeValue = 96;
            baseFile = ":/assets/progressindicator/base-96.png";
            maskFile = ":/assets/progressindicator/mask-96.png";
        }

        basePixmap.load(baseFile);
        maskPixmap.load(maskFile);
        setWidth(sizeValue);
        setHeight(sizeValue);
        update();
    }

    QString m_size;
    int m_progress;
    QPixmap basePixmap;
    QPixmap maskPixmap;
};
}
QML_DECLARE_TYPE(MeeShop::ProgressIndicator)
#endif // PROGRESSINDICATOR_H

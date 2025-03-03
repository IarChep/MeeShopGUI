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

        // Create an off-screen pixmap to combine base and mask
        QPixmap combinedPixmap(width(), height());
        combinedPixmap.fill(Qt::transparent);

        QPainter combinedPainter(&combinedPixmap);
        combinedPainter.setRenderHints(painter->renderHints());

        // Draw base
        if (!basePixmap.isNull()) {
            combinedPainter.drawPixmap(0, 0, basePixmap);
        }

        // If progress > 0, draw mask with clip path
        if (m_progress > 0) {
            QPainterPath path;
            qreal centerX = width() / 2;
            qreal centerY = height() / 2;
            qreal radius = floor(width() * sqrt(2) / 2) + 1;

            qreal startAngle = 270;
            qreal spanAngle = (m_progress / 100.0) * 360;

            // Calculate start point
            qreal startX = centerX + radius * cos(startAngle * M_PI / 180);
            qreal startY = centerY + radius * sin(startAngle * M_PI / 180);

            path.moveTo(centerX, centerY);
            path.lineTo(startX, startY);
            path.arcTo(QRectF(centerX - radius, centerY - radius, 2*radius, 2*radius), startAngle, spanAngle);
            path.closeSubpath();

            combinedPainter.setClipPath(path);
            if (!maskPixmap.isNull()) {
                qDebug() << "Drawing mask";
                combinedPainter.drawPixmap(0, 0, maskPixmap);
            } else {
                qDebug() << "Mask is null";
            }
            combinedPainter.setClipping(false);
        }

        combinedPainter.end();

        // Draw the combined pixmap with the original painter
        painter->drawPixmap(0, 0, combinedPixmap);
    }

private:
    void loadImages() {
        // Load images based on size
        QString baseFile, maskFile;
        int sizeValue = 24; // default tiny
        if (m_size == "tiny") {
            sizeValue = 24;
            baseFile = ":/ProcessIndicatorAssets/base-24.png";
            maskFile = ":/ProcessIndicatorAssets/mask-24.png";
        } else if (m_size == "small") {
            sizeValue = 32;
            baseFile = ":/ProcessIndicatorAssets/base-32.png";
            maskFile = ":/ProcessIndicatorAssets/mask-32.png";
        } else if (m_size == "medium") {
            sizeValue = 64;
            baseFile = ":/ProcessIndicatorAssets/base-64.png";
            maskFile = ":/ProcessIndicatorAssets/mask-64.png";
        } else if (m_size == "big") {
            sizeValue = 96;
            baseFile = ":/ProcessIndicatorAssets/base-96.png";
            maskFile = ":/ProcessIndicatorAssets/mask-96.png";
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

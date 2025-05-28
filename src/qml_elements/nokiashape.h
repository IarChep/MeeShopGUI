#ifndef NOKIASHAPE_H
#define NOKIASHAPE_H

#include <QDeclarativeItem>
#include <qdeclarative.h>
#include <QSvgRenderer>
#include <QRectF>
#include <QPainter>
#include <QDebug>

namespace MeeShop {
class NokiaShape : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool borderVisible READ borderVisible WRITE setBorderVisible NOTIFY borderVisibleChanged)
    Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)

public:
    NokiaShape(QDeclarativeItem *parent = 0)
        : QDeclarativeItem(parent), m_color(Qt::white), m_borderVisible(false), m_borderWidth(0), m_borderColor(Qt::black) {
        setFlag(QGraphicsItem::ItemHasNoContents, false);
    }

    QColor color() const { return m_color; }
    void setColor(const QColor &color) {
        if (color != m_color) {
            m_color = color;
            emit colorChanged(m_color);
            update();
        }
    }

    bool borderVisible() const { return m_borderVisible; }
    void setBorderVisible(bool visible) {
        if (visible != m_borderVisible) {
            m_borderVisible = visible;
            emit borderVisibleChanged(m_borderVisible);
            update();
        }
    }

    int borderWidth() const { return m_borderWidth; }
    void setBorderWidth(int width) {
        if (width != m_borderWidth) {
            m_borderWidth = width;
            emit borderWidthChanged(m_borderWidth);
            update();
        }
    }

    QColor borderColor() const { return m_borderColor; }
    void setBorderColor(const QColor &color) {
        if (color != m_borderColor) {
            m_borderColor = color;
            emit borderColorChanged(m_borderColor);
            update();
        }
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        QString svgText = "<svg xmlns=\"http://www.w3.org/2000/svg\"><path fill=\"" + m_color.name() + "\" d=\"M78,39c0,28.89-10.11,39-39,39S0,67.89,0,39,10.11,0,39,0,78,10.11,78,39Z\"/></svg>";

        if (m_borderVisible)
            svgText.replace("/>", " stroke=\"" + m_borderColor.name() + "\" stroke-width=\"" + QString::number(m_borderWidth) + "\"/>");
        QSvgRenderer renderer;
        renderer.load(svgText.toUtf8());
        renderer.render(painter, QRectF(0, 0, width(), height()));
    }

signals:
    void colorChanged(QColor color);
    void borderVisibleChanged(bool visible);
    void borderWidthChanged(int width);
    void borderColorChanged(QColor color);

private:
    QColor m_color;
    bool m_borderVisible;
    int m_borderWidth;
    QColor m_borderColor;
};
}

QML_DECLARE_TYPE(MeeShop::NokiaShape)
#endif // NOKIASHAPE_H

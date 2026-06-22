#ifndef GRADIENTER_H
#define GRADIENTER_H

#include <QDeclarativeItem>
#include <qdeclarative.h>
#include <QPainter>
#include <QLinearGradient>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QImage>
#include <QColor>
#include <QDebug>

namespace MeeShop {

// Самостоятельный QML-элемент: сам рисует вертикальный градиент,
// цвета которого берёт из уже загруженного QML Image (без повторной загрузки).
class Gradienter : public QDeclarativeItem {
    Q_OBJECT
    Q_PROPERTY(QObject* source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit Gradienter(QDeclarativeItem *parent = 0)
        : QDeclarativeItem(parent), m_source(0),
          m_topColor(QColor("#1E90FF")), m_bottomColor(QColor("#1E90FF")) {
        setFlag(QGraphicsItem::ItemHasNoContents, false);
    }

    QObject *source() const { return m_source; }
    void setSource(QObject *item) {
        if (m_source == item) return;
        m_source = item;
        emit sourceChanged();
        grabFromSource();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0.0, m_topColor);
        gradient.setColorAt(1.0, m_bottomColor);
        painter->fillRect(QRectF(0, 0, width(), height()), gradient);
    }

    // Цвет градиента на вертикальной позиции pos (0 — верх, 1 — низ).
    Q_INVOKABLE QColor colorAt(qreal pos) const {
        if (pos < 0.0) pos = 0.0;
        if (pos > 1.0) pos = 1.0;
        return QColor(
            m_topColor.red()   + qRound((m_bottomColor.red()   - m_topColor.red())   * pos),
            m_topColor.green() + qRound((m_bottomColor.green() - m_topColor.green()) * pos),
            m_topColor.blue()  + qRound((m_bottomColor.blue()  - m_topColor.blue())  * pos));
    }

    // Яркость градиента (0..1) на позиции pos. Считается в C++, т.к. в QML 1.x
    // у типа color нет доступа к компонентам r/g/b.
    Q_INVOKABLE qreal luminanceAt(qreal pos) const {
        const QColor c = colorAt(pos);
        return (0.299 * c.red() + 0.587 * c.green() + 0.114 * c.blue()) / 255.0;
    }

public slots:
    // Пересчитать цвета с текущего source (например после загрузки картинки).
    void refresh() { grabFromSource(); }

signals:
    void sourceChanged();
    void colorsChanged();

private:
    // Снимает пиксели уже отрисованного QML Image через сцену (публичный API).
    void grabFromSource() {
        QGraphicsObject *obj = qobject_cast<QGraphicsObject*>(m_source);
        if (!obj || !obj->scene())
            return;
        const QSize size = obj->boundingRect().size().toSize();
        if (size.isEmpty())
            return;

        QImage image(size, QImage::Format_ARGB32);
        image.fill(0);
        QPainter painter(&image);
        obj->scene()->render(&painter,
                             QRectF(0, 0, size.width(), size.height()),
                             obj->sceneBoundingRect(),
                             Qt::IgnoreAspectRatio);
        painter.end();

        if (applyColors(image)) {
            emit colorsChanged();
            update();
        }
    }

    bool applyColors(const QImage &image) {
        if (image.isNull() || image.width() < 1 || image.height() < 11)
            return false;
        const int middle = image.width() / 2;
        m_topColor = QColor(image.pixel(middle, 5));
        m_bottomColor = QColor(image.pixel(middle, image.height() - 5));
        m_topColor.setAlpha(255);
        m_bottomColor.setAlpha(255);
        qDebug() << "[Gradienter] grabbed colors:" << m_topColor.name() << m_bottomColor.name();
        return true;
    }

    QObject *m_source;
    QColor m_topColor;
    QColor m_bottomColor;
};

}

QML_DECLARE_TYPE(MeeShop::Gradienter)
#endif // GRADIENTER_H

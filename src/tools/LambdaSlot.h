// LambdaSlot.h
#ifndef LAMBDASLOT_H
#define LAMBDASLOT_H

#include <QObject>
#include <functional>

namespace MeeShop {
class LambdaSlot : public QObject {
    Q_OBJECT
public:
    // Конструктор принимает лямбда-функцию, которую нужно вызвать,
    // и имеет родительский объект, чтобы не заботиться о последующем удалении.
    explicit LambdaSlot(std::function<void()> func, QObject *parent = 0)
        : QObject(parent), m_func(func) {}

public slots:
    void call() {
        m_func();
        deleteLater();  // Автоматически удаляем объект после вызова
    }

private:
    std::function<void()> m_func;
};
}
#endif // LAMBDASLOT_H

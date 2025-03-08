#ifndef MEESHOP_APPLICATIONINFO_H
#define MEESHOP_APPLICATIONINFO_H

#include <QObject>
#include <QMetaType>
#include <QStringList>
#include <nlohmann/json.hpp>
#include <qdebug.h>

namespace MeeShop {

class ApplicationInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id NOTIFY appChanged)
    Q_PROPERTY(QString title READ title NOTIFY appChanged)
    Q_PROPERTY(QString publisher READ publisher NOTIFY appChanged)
    Q_PROPERTY(QString description READ description NOTIFY appChanged)
    Q_PROPERTY(QString package READ package NOTIFY appChanged)
    Q_PROPERTY(QString downloadsCount READ downloadsCount NOTIFY appChanged)
    Q_PROPERTY(int ratingCount READ ratingCount NOTIFY appChanged)
    Q_PROPERTY(QStringList screenshotsThumbnails READ screenshotsThumbnails NOTIFY appChanged)
    Q_PROPERTY(QStringList screenshots READ screenshotsThumbnails NOTIFY appChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY appChanged)
public:
    explicit ApplicationInfo(QObject *parent = 0);

    void setJson(const nlohmann::json app);

    QString id() const;
    QString title() const;
    QString publisher() const;
    QString description() const;
    QString package() const;
    QString downloadsCount() const;
    int ratingCount() const;
    QStringList screenshotsThumbnails() const;
    QStringList screenshots() const;
    QString icon() const;

signals:
    void appChanged();
private:
    nlohmann::json m_appJson;

};

} // namespace MeeShop

Q_DECLARE_METATYPE(MeeShop::ApplicationInfo*)

#endif // MEESHOP_APPLICATIONINFO_H

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

    void setJson(nlohmann::json app);

    QString id();
    QString title();
    QString publisher();
    QString description();
    QString package();
    QString downloadsCount();
    int ratingCount();
    QStringList screenshotsThumbnails();
    QStringList screenshots();
    QString icon();

signals:
    void appChanged();
private:
    nlohmann::json m_appJson;

};

} // namespace MeeShop

Q_DECLARE_METATYPE(MeeShop::ApplicationInfo*)

#endif // MEESHOP_APPLICATIONINFO_H

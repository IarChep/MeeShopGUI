#ifndef MEDIAOPENER_H
#define MEDIAOPENER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

namespace MeeShop {

// Открывает изображение (напр. скриншот приложения) во ВСТРОЕННОМ системном
// просмотрщике N9 (/usr/bin/mediaviewer). Просмотрщик умеет только ЛОКАЛЬНЫЕ файлы
// (по http-URL он сообщает «файл повреждён»), поэтому картинку сначала скачиваем во
// временный файл и открываем его. Если просмотрщика нет (симулятор) или что-то не
// удалось — откатываемся к открытию в браузере, как было раньше.
class MediaOpener : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
public:
    explicit MediaOpener(QObject *parent = nullptr);

    bool busy() const { return m_busy; }

    // Скачать (если нужно) и открыть изображение по URL во встроенном просмотрщике.
    Q_INVOKABLE void openImage(const QString &url);

signals:
    void busyChanged();
    void error(const QString &message);

private slots:
    void onDownloadFinished();

private:
    static QString viewerBinary() { return QString::fromLatin1("/usr/bin/mediaviewer"); }
    void setBusy(bool busy);
    void openLocalFile(const QString &path);
    void openInBrowserFallback(const QString &url);

    QNetworkAccessManager m_manager;
    bool m_busy;
};

}

#endif // MEDIAOPENER_H

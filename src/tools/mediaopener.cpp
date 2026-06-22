#include "mediaopener.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>

namespace MeeShop {

MediaOpener::MediaOpener(QObject *parent)
    : QObject(parent), m_busy(false)
{
}

void MediaOpener::setBusy(bool busy)
{
    if (m_busy == busy)
        return;
    m_busy = busy;
    emit busyChanged();
}

void MediaOpener::openImage(const QString &url)
{
    // Нет встроенного просмотрщика (напр. симулятор) — открываем как раньше, в браузере.
    if (!QFile::exists(viewerBinary())) {
        openInBrowserFallback(url);
        return;
    }

    const QUrl u(url);
    const QString scheme = u.scheme();
    if (scheme.isEmpty() || scheme == QLatin1String("file")) {
        openLocalFile(u.toLocalFile().isEmpty() ? url : u.toLocalFile());
        return;
    }

    // Скачиваем во временный файл, затем откроем его во встроенном просмотрщике.
    setBusy(true);
    QNetworkReply *reply = m_manager.get(QNetworkRequest(u));
    reply->setProperty("origUrl", url);
    connect(reply, SIGNAL(finished()), this, SLOT(onDownloadFinished()));
}

void MediaOpener::onDownloadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;
    reply->deleteLater();

    const QString origUrl = reply->property("origUrl").toString();

    if (reply->error() != QNetworkReply::NoError) {
        setBusy(false);
        emit error(reply->errorString());
        openInBrowserFallback(origUrl); // не вышло скачать — пусть откроет браузер
        return;
    }

    // Имя файла берём из URL (с расширением — просмотрщик ориентируется на него).
    QString name = QFileInfo(QUrl(origUrl).path()).fileName();
    if (name.isEmpty())
        name = QString::fromLatin1("image");
    if (!name.contains('.'))
        name += QString::fromLatin1(".jpg");

    const QString path = QDir::tempPath() + "/meeshop_" + name;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        setBusy(false);
        emit error(QString::fromLatin1("Cannot write temporary file"));
        openInBrowserFallback(origUrl);
        return;
    }
    f.write(reply->readAll());
    f.close();

    setBusy(false);
    openLocalFile(path);
}

void MediaOpener::openLocalFile(const QString &path)
{
    // ВАЖНО: MeeShop работает от root, а GUI-приложение, запущенное от root, не может
    // подключиться к пользовательскому themedaemon и виснет на «Failed to connect to
    // remote themedaemon. Retrying…». Поэтому запускаем просмотрщик через invoker —
    // applauncherd стартует его от имени СЕССИОННОГО пользователя (как и .desktop),
    // и тема/сессия подхватываются корректно.
    const QString invoker = QString::fromLatin1("/usr/bin/invoker");
    if (QFile::exists(invoker)) {
        if (QProcess::startDetached(invoker, QStringList()
                << QString::fromLatin1("--type=m")
                << QString::fromLatin1("--no-wait")
                << viewerBinary() << path))
            return;
    }

    // Фолбэки: прямой запуск просмотрщика, затем системное открытие файла.
    if (QProcess::startDetached(viewerBinary(), QStringList() << path))
        return;
    qWarning() << "MediaOpener: failed to launch viewer for" << path;
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MediaOpener::openInBrowserFallback(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));
}

}

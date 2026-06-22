#ifndef APT_H
#define APT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QLatin1Char>
#include <unistd.h>

#include "pseudoterminal.h"
#include "aptlogmodel.h"
#include "aptparser.h"

// Обёртка над apt-get: запускает операции и переводит вывод pty в Qt-сигналы.
// Разбор каждой строки делегирован AptLineParser (универсальная таблица QRegExp,
// см. aptparser.h): любая непустая строка либо попадает в статус/лог, либо явно
// помечена как шум перерисовки. Фиксированных switch/contains-случаев больше нет.
class AptTools : public QObject {
    Q_OBJECT
public:
    explicit AptTools(const QString &executable, QObject *parent = 0)
        : QObject(parent), m_executable(executable),
          m_lastDeterminate(false), m_lastPercent(-1) {
        connect(&m_pt, SIGNAL(lineRead(QString)), this, SLOT(onLine(QString)));
        connect(&m_pt, SIGNAL(finished(int)),     this, SLOT(onFinished(int)));
        connect(&m_pt, SIGNAL(failed(QString)),   this, SLOT(onFailed(QString)));
    }

    bool isRunning() const { return m_pt.isRunning(); }

    // Доступ к выводу: структурированная модель лога и сырой текст.
    AptLogModel *log() { return &m_log; }
    Q_INVOKABLE QString output() const { return m_output; }

public slots:
    void update()                    { run(QStringList() << "update"); }
    void install(const QString &pkg) { run(QStringList() << "install" << "-y" << "--force-yes" << pkg); }
    void remove(const QString &pkg)  { run(QStringList() << "remove" << "-y" << "--force-yes" << pkg); }
    void autoremovePurge()           { run(QStringList() << "autoremove" << "--purge" << "-y" << "--force-yes"); }

signals:
    // determinate=true -> определённый прогресс (бар); false -> неопределённый (спиннер).
    // Заменяет прежний «нюх» по подстроке "Downloading" на стороне потребителя.
    void actionChanged(const QString &action, bool determinate);
    void progress(const QString &action, int percent);
    void warning(const QString &message);
    void error(const QString &message);
    void finished(int exitCode, const QString &output);
    // Каждая записанная строка лога с уровнем (AptLogModel::Level) — для
    // маршрутизации в per-app логи очереди установки.
    void logLine(int level, const QString &message);

private slots:
    void onLine(const QString &line) {
        if (line.isEmpty())
            return;
        m_output += line;
        m_output += QLatin1Char('\n');

        const AptLineResult r = m_parser.parse(line);
        if (r.drop)
            return; // единственное место тихого отбрасывания (шум перерисовки)

        // (A) Статус: actionChanged только при смене метки ИЛИ определённости —
        // иначе кадры перерисовки прогресса дёргали бы dataChanged без нужды.
        if (r.updatesAction && (r.action != m_lastAction || r.determinate != m_lastDeterminate)) {
            m_lastAction = r.action;
            m_lastDeterminate = r.determinate;
            m_lastPercent = -1; // новая фаза -> её первый процент всегда отправить
            emit actionChanged(r.action, r.determinate);
        }

        // (B) Процент: без спама статуса, только при изменении значения.
        if (r.hasPercent && r.percent != m_lastPercent) {
            m_lastPercent = r.percent;
            emit progress(m_lastAction, r.percent);
        }

        // (C) Лог: чистая метка для распознанных правил, сырая строка для
        // catch-all/ошибок (useWholeLine). Захватываем всё, кроме явного шума.
        if (r.loggable)
            addLog(r.level, r.action);
    }

    void onFinished(int code) {
        if (code != 0)
            addLog(AptLogModel::Error, QString::fromLatin1("apt exited with code %1").arg(code));
        emit finished(code, m_output);
    }

    void onFailed(const QString &message) {
        // Сообщаем об ошибке и обязательно завершаем, чтобы UI не завис.
        addLog(AptLogModel::Error, message);
        emit error(message);
        emit finished(-1, m_output);
    }

private:
    void run(const QStringList &args) {
        if (m_pt.isRunning()) {
            emit error(QString::fromLatin1("Another APT operation is already running"));
            return;
        }
        m_output.clear();
        m_log.clear();
        // Сброс состояния дедупликации статуса/прогресса, чтобы первая строка
        // нового шага (напр. install после update) всегда дала сигнал.
        m_lastAction.clear();
        m_lastDeterminate = false;
        m_lastPercent = -1;

        if (access(m_executable.toLocal8Bit().constData(), X_OK) != 0) {
            const QString msg = QString::fromLatin1("APT executable not found or not executable: ") + m_executable;
            addLog(AptLogModel::Error, msg);
            emit error(msg);
            emit finished(-1, msg);
            return;
        }
        if (!m_pt.start(m_executable, args)) {
            const QString msg = QString::fromLatin1("Failed to start APT");
            addLog(AptLogModel::Error, msg);
            emit error(msg);
            emit finished(-1, m_output);
        }
    }

    // Пишет строку в модель лога и дублирует её сигналом для per-app логов.
    void addLog(AptLogModel::Level level, const QString &message) {
        m_log.append(level, message);
        emit logLine(int(level), message);
    }

    QString m_executable;
    QString m_output;
    AptLogModel m_log;
    PseudoTerm m_pt;

    AptLineParser m_parser;       // универсальный разбор строк (aptparser.h)
    QString m_lastAction;         // дедуп: последняя отправленная метка статуса
    bool m_lastDeterminate;       // дедуп: последняя отправленная определённость
    int m_lastPercent;            // дедуп: последний отправленный процент
};

#endif // APT_H

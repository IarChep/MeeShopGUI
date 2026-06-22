// pseudoterminal.h
#ifndef PSEUDO_TERM_H
#define PSEUDO_TERM_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <sys/types.h>

class QSocketNotifier;

// Запускает программу в псевдотерминале и читает её вывод построчно.
// Полностью в духе Qt: QObject + QSocketNotifier (читаем в цикле событий
// главного потока, без рукотворных потоков, лямбд и гонок).
class PseudoTerm : public QObject {
    Q_OBJECT
public:
    explicit PseudoTerm(QObject *parent = 0);
    ~PseudoTerm();

    bool isRunning() const { return m_pid > 0; }

    // Запускает программу. Возвращает false, если уже идёт другая команда
    // или не удалось создать процесс (тогда эмитится failed()).
    bool start(const QString &program, const QStringList &arguments);

signals:
    void lineRead(const QString &line);   // очередная строка вывода
    void finished(int exitCode);          // нормальное завершение
    void failed(const QString &error);    // не удалось запустить / убит сигналом

private slots:
    void onReadyRead();

private:
    void emitLines();
    void finish();
    void cleanup();
    static QString sanitize(const QByteArray &raw);

    int m_masterFd;
    pid_t m_pid;
    QSocketNotifier *m_notifier;
    QByteArray m_buffer;
};

#endif

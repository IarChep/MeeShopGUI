#include "pseudoterminal.h"

#include <QSocketNotifier>
#include <QRegExp>

#include <pty.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <cerrno>
#include <cstring>
#include <vector>

PseudoTerm::PseudoTerm(QObject *parent)
    : QObject(parent), m_masterFd(-1), m_pid(-1), m_notifier(0) {
}

PseudoTerm::~PseudoTerm() {
    if (m_pid > 0) {
        ::kill(m_pid, SIGTERM);
        int status;
        while (::waitpid(m_pid, &status, 0) < 0 && errno == EINTR) {}
    }
    if (m_masterFd != -1)
        ::close(m_masterFd);
}

bool PseudoTerm::start(const QString &program, const QStringList &arguments) {
    if (m_pid > 0)
        return false; // уже занят

    // argv готовим ДО fork, чтобы в ребёнке не выделять память.
    std::vector<QByteArray> storage;
    storage.push_back(program.toLocal8Bit());
    for (int i = 0; i < arguments.size(); ++i)
        storage.push_back(arguments.at(i).toLocal8Bit());

    std::vector<char*> argv;
    for (std::size_t i = 0; i < storage.size(); ++i)
        argv.push_back(storage[i].data());
    argv.push_back(0);

    int fd = -1;
    pid_t pid = forkpty(&fd, 0, 0, 0);
    if (pid < 0) {
        emit failed(QString::fromLatin1("forkpty: ") + strerror(errno));
        return false;
    }

    if (pid == 0) {
        // Дочерний процесс: никакого Qt, только exec.
        // Закрываем унаследованные дескрипторы (кроме pty 0/1/2): сокет дисплея/
        // компоновщика, нотифаеры Qt, сокет inputContext и т.п. Это гигиена exec —
        // не передаём ресурсы GUI в apt-процесс. ПРИМЕЧАНИЕ: раньше тут было сказано,
        // что без этого «пропадает тулбар/статус-бар» — это оказалось НЕВЕРНО.
        // Настоящая причина исчезновения интерфейса была утечкой состояния клипа в
        // ProgressIndicator::paint() (см. ../../qml_elements/progressindicator.h),
        // а не наследование fd. Цикл оставлен как полезная гигиена.
        int maxFd = static_cast<int>(sysconf(_SC_OPEN_MAX));
        if (maxFd < 0)
            maxFd = 1024; // лимит неопределён — берём разумный максимум
        for (int fd = 3; fd < maxFd; ++fd)
            ::close(fd);
        setenv("LANG", "C", 1);
        setenv("TERM", "dumb", 1);
        execvp(argv[0], argv.data());
        _exit(127); // exec не удался — родитель увидит код 127
    }

    m_pid = pid;
    m_masterFd = fd;
    fcntl(m_masterFd, F_SETFL, O_NONBLOCK);
    m_buffer.clear();
    m_notifier = new QSocketNotifier(m_masterFd, QSocketNotifier::Read, this);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(onReadyRead()));
    return true;
}

void PseudoTerm::onReadyRead() {
    char buf[1024];
    for (;;) {
        ssize_t n = ::read(m_masterFd, buf, sizeof(buf));
        if (n > 0) {
            m_buffer.append(buf, int(n));
            emitLines();
            continue;
        }
        if (n < 0 && errno == EINTR)
            continue;
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return; // данных пока нет — ждём следующего события
        break;      // n == 0 (EOF) либо EIO при закрытии slave — конец
    }

    // Хвост без перевода строки.
    if (!m_buffer.isEmpty()) {
        const QString line = sanitize(m_buffer);
        m_buffer.clear();
        if (!line.isEmpty())
            emit lineRead(line);
    }
    finish();
}

void PseudoTerm::emitLines() {
    for (;;) {
        const int nl = m_buffer.indexOf('\n');
        const int cr = m_buffer.indexOf('\r');
        int idx;
        if (nl == -1 && cr == -1)
            break;
        else if (nl == -1)
            idx = cr;
        else if (cr == -1)
            idx = nl;
        else
            idx = qMin(nl, cr);

        const QByteArray segment = m_buffer.left(idx);
        m_buffer.remove(0, idx + 1);
        const QString line = sanitize(segment);
        if (!line.isEmpty())
            emit lineRead(line);
    }
}

void PseudoTerm::finish() {
    const pid_t pid = m_pid;
    int status = 0;
    pid_t w;
    do {
        w = ::waitpid(pid, &status, 0);
    } while (w < 0 && errno == EINTR);

    cleanup(); // закрываем fd, удаляем notifier, сбрасываем pid (isRunning()==false)

    if (w > 0 && WIFEXITED(status))
        emit finished(WEXITSTATUS(status));
    else if (w > 0 && WIFSIGNALED(status))
        emit failed(QString::fromLatin1("terminated by signal %1").arg(WTERMSIG(status)));
    else
        emit finished(-1);
}

void PseudoTerm::cleanup() {
    if (m_notifier) {
        m_notifier->setEnabled(false);
        m_notifier->deleteLater(); // мы внутри его слота — удаляем отложенно
        m_notifier = 0;
    }
    if (m_masterFd != -1) {
        ::close(m_masterFd);
        m_masterFd = -1;
    }
    m_pid = -1;
}

QString PseudoTerm::sanitize(const QByteArray &raw) {
    QString line = QString::fromUtf8(raw);
    static const QRegExp ansi(QString(QChar(0x1B)) + QString::fromLatin1("\\[[0-?]*[ -/]*[@-~]"));
    line.remove(ansi);
    return line.trimmed();
}

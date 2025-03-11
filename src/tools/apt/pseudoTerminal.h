// pseudoTerminal.h
#ifndef PSEUDO_TERM
#define PSEUDO_TERM

#include <iostream>
#include <unistd.h>
#include <pty.h>
#include <utmp.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <regex>

enum OutputMode { Trim, AsIs };

class PseudoTerm {
public:
    PseudoTerm(OutputMode outputMode);
    ~PseudoTerm();

    // Запускает программу с указанными аргументами через forkpty.
    void run_command(const std::string& programPath, const std::vector<std::string>& args);

    // Callback-обработчики событий.
    // Вызываются при получении строки вывода (всегда через process_line)
    std::function<void(const std::string&)> OnOutputReceived;
    std::function<void(int)> OnProgramExited;
    std::function<void(int)> OnProgramErrored;

private:
    int master_fd;    // Дескриптор мастера PTY.
    pid_t pid;        // PID дочернего процесса.
    OutputMode outputMode;

    // Функция обработки строки (оставляем без изменений).
    void process_line(std::string line) {
        if (outputMode == Trim) {
            // Удалить все непечатаемые символы с начала строки
            while (!line.empty() &&
                   (static_cast<unsigned char>(line.front()) < 32 ||
                    static_cast<unsigned char>(line.front()) > 126)) {
                line.erase(0, 1);
            }
            // Удалить все непечатаемые символы с конца строки
            while (!line.empty() &&
                   (static_cast<unsigned char>(line.back()) < 32 ||
                    static_cast<unsigned char>(line.back()) > 126)) {
                line.pop_back();
            }

            // Удалить ANSI escape-коды из строки
            std::regex ansi_regex(R"(\x1B

\[[0-?]*[ -/]*[@-~])");
            line = std::regex_replace(line, ansi_regex, "");
        }
        // Вызов общего callback, даже для пустых строк.
        if (OnOutputReceived) {
            OnOutputReceived(line);
        }
    }
};

#endif

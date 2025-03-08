#include "pseudoTerminal.h"

PseudoTerm::PseudoTerm(OutputMode outputMode)
    : master_fd(-1), slave_fd(-1), pid(-1), outputMode(outputMode) {

    // Открытие псевдотерминала
    master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (master_fd == -1) {
        throw_runtime_error("Error opening master PTY: " + std::string(strerror(errno)));
    }

    if (grantpt(master_fd) == -1) {
        close(master_fd);
        throw_runtime_error("Error granting PTY: " + std::string(strerror(errno)));
    }

    if (unlockpt(master_fd) == -1) {
        close(master_fd);
        throw_runtime_error("Error unlocking PTY: " + std::string(strerror(errno)));
    }

    if (ptsname_r(master_fd, slave_name, sizeof(slave_name)) != 0) {
        close(master_fd);
        throw_runtime_error("Error getting slave PTY name: " + std::string(strerror(errno)));
    }
}

PseudoTerm::~PseudoTerm() {
    close_terminal();
}

void PseudoTerm::close_terminal() {
    if (master_fd != -1) {
        close(master_fd);
        master_fd = -1;
    }
}

void PseudoTerm::throw_runtime_error(const std::string& msg) {
    throw std::runtime_error(msg);
}

void PseudoTerm::run_command(const std::string& programPath, const std::vector<std::string>& args) {
    pid = fork();
    if (pid == -1) {
        close_terminal();
        throw_runtime_error("Error forking: " + std::string(strerror(errno)));
    }

    if (pid == 0) {
        setsid();
        slave_fd = open(slave_name, O_RDWR);
        if (slave_fd == -1) {
            if (OnProgramErrored) {
                OnProgramErrored(errno);
            }
            std::cerr << "Error opening slave PTY: " << strerror(errno) << std::endl;
            exit(1);
        }

        dup2(slave_fd, STDIN_FILENO);
        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDERR_FILENO);
        close(slave_fd);

        setenv("LANG", "en", 1);

        std::vector<const char*> exec_args;
        exec_args.push_back(programPath.c_str());
        for (const std::string& arg : args) {
            exec_args.push_back(arg.c_str());
        }
        exec_args.push_back(nullptr);

        execvp(programPath.c_str(), const_cast<char* const*>(exec_args.data()));
        if (OnProgramErrored) {
            OnProgramErrored(errno);
        }
        exit(1);
    } else {
        fd_set read_fds;
        char buffer[256];
        ssize_t n;
        std::string current_line;

        while (true) {
            FD_ZERO(&read_fds);
            FD_SET(master_fd, &read_fds);
            FD_SET(STDIN_FILENO, &read_fds);

            int max_fd = std::max(master_fd, STDIN_FILENO) + 1;
            int ready = select(max_fd, &read_fds, nullptr, nullptr, nullptr);

            if (ready == -1) {
                throw_runtime_error("Error in select: " + std::string(strerror(errno)));
            }

            if (FD_ISSET(master_fd, &read_fds)) {
                n = read(master_fd, buffer, sizeof(buffer));
                if (n <= 0) break;

                for (ssize_t i = 0; i < n; ++i) {
                    if (buffer[i] == '\n' || buffer[i] == '\r') {
                        if (!current_line.empty()) {
                            process_line(current_line);
                            current_line.clear();
                        }
                        // Если это \r, сразу начинаем новую строку
                        if (buffer[i] == '\r') continue;
                    } else {
                        current_line += buffer[i];
                    }
                }
            }

            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                n = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (n <= 0) break;
                write(master_fd, buffer, n);
            }
        }

        // Обработать остаток строки, если он есть
        if (!current_line.empty()) {
            process_line(current_line);
        }

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && OnProgramExited) {
            OnProgramExited(WEXITSTATUS(status));
        }
    }
}

void PseudoTerm::process_line(std::string line) {
    if (outputMode == OutputMode::Trim) {
        // Удалить все непечатаемые символы с начала строки
        while (!line.empty() && (static_cast<unsigned char>(line.front()) < 32 || static_cast<unsigned char>(line.front()) > 126)) {
            line.erase(0, 1);
        }
        // Удалить все непечатаемые символы с конца строки
        while (!line.empty() && (static_cast<unsigned char>(line.back()) < 32 || static_cast<unsigned char>(line.back()) > 126)) {
            line.pop_back();
        }

        // Удалить ANSI escape-коды из строки
        std::regex ansi_regex(R"(\x1B\[[0-?]*[ -/]*[@-~])");
        line = std::regex_replace(line, ansi_regex, "");
    }

    // Вызвать OnOutputReceived даже для пустых строк
    if (OnOutputReceived) {
        OnOutputReceived(line);
    }
}

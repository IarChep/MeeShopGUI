#include "pseudoTerminal.h"

PseudoTerm::PseudoTerm(OutputMode outputMode)
    : master_fd(-1), pid(-1), outputMode(outputMode) {
}

PseudoTerm::~PseudoTerm() {
    if (master_fd != -1) {
        close(master_fd);
        master_fd = -1;
    }
}

void PseudoTerm::run_command(const std::string& programPath, const std::vector<std::string>& args) {
    // Создаем псевдотерминал через forkpty.
    pid = forkpty(&master_fd, nullptr, nullptr, nullptr);
    if (pid < 0) {
        throw std::runtime_error("Error in forkpty: " + std::string(strerror(errno)));
    }

    if (pid == 0) {
        setenv("LANG", "en", 1);
        setenv("TERM", "xterm", 1);

        std::vector<const char*> exec_args;
        exec_args.push_back(programPath.c_str());
        for (const std::string& arg : args) {
            exec_args.push_back(arg.c_str());
        }
        exec_args.push_back(nullptr);

        execvp(programPath.c_str(), const_cast<char* const*>(exec_args.data()));
        // Если execvp возвращает, произошла ошибка
        if (OnProgramErrored) {
            OnProgramErrored(errno);
        }
        std::cerr << "Error executing command: " << strerror(errno) << std::endl;
        exit(1);
    }
    else {
        // Родительский процесс. Чтение вывода из master_fd по мере его поступления.
        fd_set read_fds;
        char buffer[256];
        ssize_t n;
        std::string current_line;

        while (true) {
            FD_ZERO(&read_fds);
            FD_SET(master_fd, &read_fds);
            int ready = select(master_fd + 1, &read_fds, nullptr, nullptr, nullptr);
            if (ready == -1) {
                throw std::runtime_error("Error in select: " + std::string(strerror(errno)));
            }

            if (FD_ISSET(master_fd, &read_fds)) {
                n = read(master_fd, buffer, sizeof(buffer));
                if (n <= 0) {
                    break;  // Вывод завершён.
                }
                for (ssize_t i = 0; i < n; ++i) {
                    if (buffer[i] == '\n' || buffer[i] == '\r') {
                        if (!current_line.empty()) {
                            process_line(current_line);
                            current_line.clear();
                        }
                        // Если символ — '\r', сразу начинаем новую строку.
                        if (buffer[i] == '\r')
                            continue;
                    } else {
                        current_line += buffer[i];
                    }
                }
            }
        }
        // Если осталась неполная строка, обрабатываем её.
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

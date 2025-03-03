#ifndef PSEUDO_TERM
#define PSEUDO_TERM

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iomanip>
#include <cerrno>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <regex>


enum OutputMode { Trim, AsIs };

class PseudoTerm {
public:

    PseudoTerm(OutputMode outputMode);
    ~PseudoTerm();
    void run_command(const std::string& programPath, const std::vector<std::string>& args);

    // Обработчики событий
    std::function<void(const std::string&)> OnOutputReceived;
    std::function<void(int)> OnProgramExited;
    std::function<void(int)> OnProgramErrored;

private:
    int master_fd;
    int slave_fd;
    char slave_name[100];
    pid_t pid;

    OutputMode outputMode;
    std::string username;

    void close_terminal();
    void throw_runtime_error(const std::string& msg);
    void request_root_password();
    void process_line(std::string line);
};

#endif

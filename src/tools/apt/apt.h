#ifndef APT_H
#define APT_H
#include <regex>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include "pseudoTerminal.h"

enum class AptEventType {
    READING_PACKAGE_LISTS,
    BUILDING_DEPENDENCY_TREE,
    READING_STATE_INFO,
    READING_DATABASE,
    UNPACKING,
    SETTING_UP,
    DOWNLOADING,
    HIT,
    DONE,
    REMOVING,
    UNKNOWN
};

struct AptEvent {
    AptEventType type;
    std::string description;
    int progress = -1;
    std::string package_name;
};

struct PackageInfo {
    int upgraded = 0;
    int newly_installed = 0;
    int to_remove = 0;
    int not_upgraded = 0;
    std::vector<std::string> additional_packages;
    std::vector<std::string> suggested_packages;
    std::vector<std::string> new_packages;
};

class AptTools {
private:
    PseudoTerm pt;
    std::string apt_executable;
    std::string apt_out;
    std::unordered_map<AptEventType, AptEvent> current_events;
    AptEventType current_active_event = AptEventType::UNKNOWN;

    // Коллбэки для уведомлений
    std::function<void(const std::string&, int)> onProgressChanged;
    std::function<void(const std::string&)> onActionChanged;
    std::function<void(const std::string&, const std::string&)> onErrorOrWarning;
    std::function<void(int, const std::string&)> onExited; // Новый обработчик

    AptEventType identifyEvent(const std::string& line) {
        if (line.find("Reading package lists...") != std::string::npos)
            return AptEventType::READING_PACKAGE_LISTS;
        if (line.find("Building dependency tree...") != std::string::npos)
            return AptEventType::BUILDING_DEPENDENCY_TREE;
        if (line.find("Reading state information...") != std::string::npos)
            return AptEventType::READING_STATE_INFO;
        if (line.find("Reading database ...") != std::string::npos)
            return AptEventType::READING_DATABASE;
        if (line.find("Unpacking ") != std::string::npos)
            return AptEventType::UNPACKING;
        if (line.find("Setting up ") != std::string::npos)
            return AptEventType::SETTING_UP;
        if (line.find("Get:") != std::string::npos || line.find("% [") != std::string::npos)
            return AptEventType::DOWNLOADING;
        if (line.find("Hit:") != std::string::npos)
            return AptEventType::HIT;
        if (line.find("Done") != std::string::npos)
            return AptEventType::DONE;
        if (line.find("Removing ") != std::string::npos)
            return AptEventType::REMOVING;
        return AptEventType::UNKNOWN;
    }

    int extractProgress(const std::string& line) {
        std::regex progress_regex("(\\d+)%");
        std::smatch match;
        if (std::regex_search(line, match, progress_regex)) {
            return std::stoi(match[1]);
        }
        return -1;
    }

    std::string extractPackageName(const std::string& line) {
        std::regex package_regex("(Unpacking|Setting up|Removing) ([\\w-]+)");
        std::smatch match;
        if (std::regex_search(line, match, package_regex)) {
            return match[2];
        }
        return "";
    }

    void processEventLine(const std::string& line, AptEventType event_type) {
        int progress = extractProgress(line);
        std::string package_name = extractPackageName(line);

        if (event_type == AptEventType::DONE) {
            if (current_active_event != AptEventType::UNKNOWN) {
                auto& event = current_events[current_active_event];
                event.progress = 100;
                if (onProgressChanged) {
                    onProgressChanged(getEventName(event.type), 100);
                }
                current_events.erase(current_active_event);
                current_active_event = AptEventType::UNKNOWN;
            }
            return;
        }

        auto it = current_events.find(event_type);
        if (it == current_events.end()) {
            AptEvent event;
            event.type = event_type;
            event.description = line;
            event.progress = progress;
            event.package_name = package_name;
            current_events[event_type] = event;
            current_active_event = event_type;
            if (onActionChanged) {
                onActionChanged(getEventName(event_type) + (package_name.empty() ? "" : " " + package_name));
            }
        } else {
            it->second.progress = progress;
            it->second.package_name = package_name;
        }

        if (progress != -1 && onProgressChanged) {
            onProgressChanged(getEventName(event_type), progress);
        }
    }

    std::string getEventName(AptEventType type) {
        switch (type) {
            case AptEventType::READING_PACKAGE_LISTS: return "Reading package lists";
            case AptEventType::BUILDING_DEPENDENCY_TREE: return "Building dependency tree";
            case AptEventType::READING_STATE_INFO: return "Reading state information";
            case AptEventType::READING_DATABASE: return "Reading database";
            case AptEventType::UNPACKING: return "Unpacking";
            case AptEventType::SETTING_UP: return "Setting up";
            case AptEventType::DOWNLOADING: return "Downloading";
            case AptEventType::HIT: return "Hit repository";
            case AptEventType::DONE: return "Done";
            case AptEventType::REMOVING: return "Removing";
            default: return "Unknown";
        }
    }

    void setupPseudoTerminalCallbacks() {
        pt.OnOutputReceived = [this](const std::string& out) {
            processLine(out);
        };
        pt.OnProgramExited = [this](const int& code) {
            if (code != 0 && onErrorOrWarning) {
                onErrorOrWarning("Error", "APT command exited with code: " + std::to_string(code));
            } else if (onExited) {
                onExited(code, apt_out);
            }
        };
    }

public:
    AptTools(const std::string apt_exe) : pt(OutputMode::Trim), apt_executable(apt_exe) {
        setupPseudoTerminalCallbacks();
    }

    void setOnProgressChanged(std::function<void(const std::string&, int)> callback) {
        onProgressChanged = callback;
    }

    void setOnActionChanged(std::function<void(const std::string&)> callback) {
        onActionChanged = callback;
    }

    void setOnErrorOrWarning(std::function<void(const std::string&, const std::string&)> callback) {
        onErrorOrWarning = callback;
    }

    void setOnExited(std::function<void(int, const std::string&)> callback) {
        onExited = callback;
    }

    void updateRepos() {
        apt_out = "";
        pt.run_command(apt_executable, {"update"});
    }

    void installPackage(const std::string& package) {
        apt_out = "";
        pt.run_command(apt_executable, {"install", "-y", package});
    }

    void removePackage(const std::string& package) {
        apt_out = "";
        pt.run_command(apt_executable, {"remove", "-y", package});
    }

    void autoremovePurge() {
        apt_out = "";
        pt.run_command(apt_executable, {"autoremove", "--purge", "-y"});
    }

    PackageInfo simulateInstall(const std::string& package) {
        PackageInfo result;
        apt_out = "";

        auto original_handler = pt.OnOutputReceived;
        pt.OnOutputReceived = [this](const std::string& out) {
            apt_out += out + "\n";
        };
        pt.run_command(apt_executable, {"install", "--dry-run", package});
        pt.OnOutputReceived = original_handler;

        std::istringstream iss(apt_out);
        std::string line;
        std::vector<std::string>* current_list = nullptr;

        std::regex summary_regex(R"(\s*(\d+)\s+upgraded,\s*(\d+)\s+newly installed,\s*(\d+)\s+to remove and\s*(\d+)\s+not upgraded)");

        while (std::getline(iss, line)) {
            std::smatch match;

            if (std::regex_search(line, match, summary_regex)) {
                result.upgraded = std::stoi(match[1]);
                result.newly_installed = std::stoi(match[2]);
                result.to_remove = std::stoi(match[3]);
                result.not_upgraded = std::stoi(match[4]);
            }
            else if (line.find("The following additional packages will be installed:") != std::string::npos) {
                current_list = &result.additional_packages;
            }
            else if (line.find("Suggested packages:") != std::string::npos) {
                current_list = &result.suggested_packages;
            }
            else if (line.find("The following NEW packages will be installed:") != std::string::npos) {
                current_list = &result.new_packages;
            }
            else if (current_list && !line.empty() && line[0] == ' ') {
                std::istringstream package_stream(line);
                std::string pkg;
                while (package_stream >> pkg) {
                    current_list->push_back(pkg);
                }
            }
            else if (current_list && !line.empty() && line[0] != ' ') {
                current_list = nullptr;
            }
        }

        return result;
    }

    bool isInstalled(const std::string& package) {
        apt_out = "";
        auto original_handler = pt.OnOutputReceived;

        pt.OnOutputReceived = [this](const std::string& out) {
            apt_out += out + "\n";
        };
        pt.run_command("/bin/dpkg", {"-s", package});
        pt.OnOutputReceived = original_handler;

        return apt_out.find("Status: install ok installed") != std::string::npos;
    }

    void processLine(const std::string& line) {
        if (line.empty()) return;
        apt_out += line + "\n";

        AptEventType event_type = identifyEvent(line);
        if (event_type != AptEventType::UNKNOWN) {
            processEventLine(line, event_type);
        } else if (line.find("E: ") != std::string::npos && onErrorOrWarning) {
            onErrorOrWarning("Error", line);
        } else if (line.find("W: ") != std::string::npos && onErrorOrWarning) {
            onErrorOrWarning("Warning", line);
        }
    }
};
#endif

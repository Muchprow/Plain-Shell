#include "Shell.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <filesystem>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string CYAN    = "\033[36m";
    const std::string BOLD    = "\033[1m";
}

Shell::Shell() : is_running(true) {}

void Shell::setup_env_path() {
#if defined(_WIN32)
    wchar_t exe_path[MAX_PATH];
    GetModuleFileNameW(NULL, exe_path, MAX_PATH);
    fs::path p(exe_path);
    std::string dir = p.parent_path().string();

    std::string cmd = "powershell -Command \"[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path', 'User') + ';" + dir + "', 'User')\"";
    std::system(cmd.c_str());
    std::cout << Color::GREEN << "[Plain] Added '" << dir << "' to User PATH successfully!\n" << Color::RESET;
#else
    std::cout << Color::YELLOW << "[Plain] PATH setup command is only implemented for Windows.\n" << Color::RESET;
#endif
}

std::vector<std::string> Shell::tokenize(const std::string& line) const {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token) tokens.push_back(token);
    return tokens;
}

bool Shell::handle_builtin_commands(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return false;

    const std::string& cmd = tokens[0];

    if (cmd == "help") {
        dictionary.print_help();
        std::cout << Color::GREEN << "  history            " << Color::RESET << "- View command history\n";
        std::cout << Color::GREEN << "  addons             " << Color::RESET << "- Manage theme addons\n";
        std::cout << Color::GREEN << "  packs              " << Color::RESET << "- Manage command packs (.plainc)\n";
        std::cout << Color::GREEN << "  setup path         " << Color::RESET << "- Register Plain Shell to System PATH\n";

        pack_manager.print_active_pack_commands();
        std::cout << "\n";
        return true;
    }

    if (cmd == "setup" && tokens.size() >= 2 && tokens[1] == "path") {
        setup_env_path();
        return true;
    }

    if (cmd == "addons") {
        if (tokens.size() == 1) {
            addon_manager.print_addons_list();
            return true;
        }

        if (tokens.size() >= 2 && tokens[1] == "disable") {
            addon_manager.disable_addon();
            std::cout << Color::GREEN << "[Plain] Switched to default theme!\n" << Color::RESET;
            return true;
        }

        if (tokens.size() >= 3 && tokens[1] == "enable") {
            if (addon_manager.enable_addon(tokens[2])) {
                std::cout << Color::GREEN << "[Plain] Addon '" << tokens[2] << "' enabled!\n" << Color::RESET;
                std::string welcome = addon_manager.get_welcome_message();
                if (!welcome.empty()) std::cout << welcome << "\n";
                addon_manager.trigger_event("on_startup");
            } else {
                std::cout << Color::RED << "[Error] Addon '" << tokens[2] << "' not found!\n" << Color::RESET;
            }
            return true;
        }
    }

    if (cmd == "packs") {
        if (tokens.size() == 1) {
            pack_manager.print_packs_list();
            return true;
        }

        if (tokens.size() >= 3 && tokens[1] == "enable") {
            if (pack_manager.enable_pack(tokens[2])) {
                std::cout << Color::GREEN << "[Plain] Pack '" << tokens[2] << "' enabled!\n" << Color::RESET;
            } else {
                std::cout << Color::RED << "[Error] Pack '" << tokens[2] << "' not found!\n" << Color::RESET;
            }
            return true;
        }

        if (tokens.size() >= 3 && tokens[1] == "disable") {
            if (pack_manager.disable_pack(tokens[2])) {
                std::cout << Color::GREEN << "[Plain] Pack '" << tokens[2] << "' disabled!\n" << Color::RESET;
            } else {
                std::cout << Color::RED << "[Error] Pack '" << tokens[2] << "' not found!\n" << Color::RESET;
            }
            return true;
        }
    }

    if (cmd == "history") {
        history_manager.print_history();
        return true;
    }

    if (cmd == "goto" || cmd == "cd") {
        if (tokens.size() < 2) {
            std::cout << Color::YELLOW << "Usage: goto <folder_path>\n" << Color::RESET;
            return true;
        }
        try {
            fs::current_path(tokens[1]);
        } catch (const fs::filesystem_error&) {
            std::cout << Color::RED << "[Error] Folder '" << tokens[1] << "' not found!\n" << Color::RESET;
        }
        return true;
    }

    if (cmd == "read") {
        if (tokens.size() < 2) {
            std::cout << Color::YELLOW << "Usage: read <filename>\n" << Color::RESET;
            return true;
        }
        std::ifstream file(tokens[1]);
        if (!file.is_open()) {
            std::cout << Color::RED << "[Error] Cannot open file '" << tokens[1] << "'!\n" << Color::RESET;
            return true;
        }
        std::cout << Color::CYAN << "--- Content of " << tokens[1] << " ---\n" << Color::RESET;
        std::string line;
        while (std::getline(file, line)) std::cout << line << "\n";
        std::cout << Color::CYAN << "-------------------------\n" << Color::RESET;
        return true;
    }

    return false;
}

void Shell::run() {
    std::string welcome = addon_manager.get_welcome_message();

    if (!welcome.empty()) {
        std::cout << welcome << "\n\n";
    } else {
        std::cout << Color::CYAN << Color::BOLD;
        std::cout << "=======================================\n";
        std::cout << "       Welcome to Plain Shell!         \n";
        std::cout << "  Type 'help' to see available commands\n";
        std::cout << "=======================================\n\n";
        std::cout << Color::RESET;
    }

    addon_manager.trigger_event("on_startup");

    std::string line;
    while (is_running) {
        std::string current_folder = fs::current_path().filename().string();
        if (current_folder.empty()) current_folder = "/";

        std::string custom_prompt = addon_manager.get_prompt(current_folder);

        if (!custom_prompt.empty()) {
            std::cout << custom_prompt;
        } else {
            std::cout << Color::BLUE << "[" << current_folder << "] " 
                      << Color::GREEN << "plain> " 
                      << Color::RESET;
        }

        if (!std::getline(std::cin, line) || line == "exit" || line == "quit") {
            is_running = false;
            break;
        }

        if (line.empty()) continue;

        history_manager.add(line);
        auto tokens = tokenize(line);

        if (handle_builtin_commands(tokens)) continue;

        if (pack_manager.has_command(line)) {
            std::string sys_cmd = pack_manager.get_command(line);
            std::system(sys_cmd.c_str());
            continue;
        }

        if (addon_manager.has_addon_command(line)) {
            std::string addon_cmd = addon_manager.get_addon_command(line);
            std::system(addon_cmd.c_str());
            continue;
        }

        std::string sys_cmd = dictionary.translate(tokens);
        if (!sys_cmd.empty()) {
            std::system(sys_cmd.c_str());
        }
    }
}

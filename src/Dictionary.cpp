#include "Dictionary.hpp"
#include <iostream>

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string GREEN   = "\033[32m";
    const std::string CYAN    = "\033[36m";
    const std::string BOLD    = "\033[1m";
}

Dictionary::Dictionary() {
    win_map["show"]   = "dir";
    win_map["where"]  = "cd";

    unix_map["show"]  = "ls -lh";
    unix_map["where"] = "pwd";
}

void Dictionary::print_help() const {
    std::cout << Color::CYAN << Color::BOLD << "\n--- Plain Shell Command Reference ---\n" << Color::RESET;
    std::cout << Color::GREEN << "  show               " << Color::RESET << "- List files in current directory\n";
    std::cout << Color::GREEN << "  where              " << Color::RESET << "- Show current working directory\n";
    std::cout << Color::GREEN << "  goto <path>        " << Color::RESET << "- Change current directory\n";
    std::cout << Color::GREEN << "  read <file>        " << Color::RESET << "- Display content of a file\n";
    std::cout << Color::GREEN << "  create <file>      " << Color::RESET << "- Create a new empty file\n";
    std::cout << Color::GREEN << "  make folder <name> " << Color::RESET << "- Create a new folder\n";
    std::cout << Color::GREEN << "  exit / quit        " << Color::RESET << "- Close Plain Shell\n\n";
}

std::string Dictionary::translate(const std::vector<std::string>& tokens) const {
    if (tokens.empty()) return "";

    std::string first_word = tokens[0];

    #if defined(_WIN32)
        auto it = win_map.find(first_word);
        if (it != win_map.end()) return it->second;
    #else
        auto it = unix_map.find(first_word);
        if (it != unix_map.end()) return it->second;
    #endif

    if (first_word == "create" && tokens.size() > 1) {
        #if defined(_WIN32)
            return "type nul > " + tokens[1];
        #else
            return "touch " + tokens[1];
        #endif
    }

    if (first_word == "make" && tokens.size() > 2 && tokens[1] == "folder") {
        return "mkdir " + tokens[2];
    }

    std::string full_cmd;
    for (const auto& t : tokens) full_cmd += t + " ";
    return full_cmd;
}

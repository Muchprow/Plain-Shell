#include "HistoryManager.hpp"
#include "Utils.hpp"
#include <iostream>
#include <fstream>

namespace Color {
    const std::string RESET = "\033[0m";
    const std::string CYAN  = "\033[36m";
    const std::string GRAY  = "\033[90m";
}

HistoryManager::HistoryManager() {
    history_file = get_base_dir() + "/history.log";
    load();
}

void HistoryManager::load() {
    std::ifstream file(history_file);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) history.push_back(line);
    }
}

void HistoryManager::add(const std::string& command) {
    if (command.empty()) return;
    history.push_back(command);
    save_line(command);
}

void HistoryManager::save_line(const std::string& command) {
    std::ofstream file(history_file, std::ios::app);
    if (file.is_open()) file << command << "\n";
}

void HistoryManager::print_history() const {
    std::cout << Color::CYAN << "\n--- Command History ---\n" << Color::RESET;
    for (size_t i = 0; i < history.size(); ++i) {
        std::cout << Color::GRAY << " " << i + 1 << "  " << Color::RESET << history[i] << "\n";
    }
    std::cout << Color::CYAN << "-----------------------\n\n" << Color::RESET;
}
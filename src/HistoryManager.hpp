#pragma once
#include <string>
#include <vector>

class HistoryManager {
public:
    HistoryManager();
    void add(const std::string& command);
    void print_history() const;

private:
    std::string history_file;
    std::vector<std::string> history;
    void load();
    void save_line(const std::string& command);
};
#pragma once
#include "Dictionary.hpp"
#include "AddonManager.hpp"
#include "HistoryManager.hpp"
#include "PackManager.hpp"
#include <vector>
#include <string>

class Shell {
public:
    Shell();
    void run();

private:
    Dictionary dictionary;
    AddonManager addon_manager;
    HistoryManager history_manager;
    PackManager pack_manager;
    bool is_running;

    std::vector<std::string> tokenize(const std::string& line) const;
    bool handle_builtin_commands(const std::vector<std::string>& tokens);
    void setup_env_path();
};
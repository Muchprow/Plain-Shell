#pragma once
#include <string>
#include <unordered_map>

struct Addon {
    std::string name;
    std::string prompt_format;
    std::string welcome_message;
    std::unordered_map<std::string, std::string> commands;
    std::unordered_map<std::string, std::string> hooks;
};

class AddonManager {
public:
    AddonManager();
    void scan_addons();
    bool enable_addon(const std::string& name);
    void disable_addon();

    bool is_addon_active() const;
    std::string get_active_addon_name() const;
    std::string get_prompt(const std::string& current_folder) const;
    std::string get_welcome_message() const;
    void trigger_event(const std::string& event_name);
    
    bool has_addon_command(const std::string& phrase) const;
    std::string get_addon_command(const std::string& phrase) const;
    void print_addons_list() const;

private:
    std::string addons_dir;
    std::string config_file;
    std::string active_addon_name;
    std::unordered_map<std::string, Addon> loaded_addons;

    void save_active_config();
    void load_active_config();
    std::string parse_ansi_colors(const std::string& input) const;
};
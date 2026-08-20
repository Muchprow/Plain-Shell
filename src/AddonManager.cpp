#include "AddonManager.hpp"
#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace Color {
    const std::string RESET  = "\033[0m";
    const std::string GREEN  = "\033[32m";
    const std::string CYAN   = "\033[36m";
    const std::string GRAY   = "\033[90m";
}

AddonManager::AddonManager() : active_addon_name("") {
    std::string base_dir = get_base_dir();
    addons_dir = base_dir + "/addons";
    config_file = base_dir + "/active_addon";

    try { fs::create_directories(addons_dir); } catch (...) {}

    scan_addons();
    load_active_config();
}

std::string AddonManager::parse_ansi_colors(const std::string& input) const {
    std::string res = input;
    size_t pos = 0;
    while ((pos = res.find("\\033", pos)) != std::string::npos) {
        res.replace(pos, 4, "\033");
        pos += 1;
    }
    return res;
}

void AddonManager::scan_addons() {
    loaded_addons.clear();
    if (!fs::exists(addons_dir)) return;

    for (const auto& entry : fs::directory_iterator(addons_dir)) {
        if (entry.path().extension() == ".plainmod" || entry.path().extension() == ".plain") {
            std::string addon_name = entry.path().stem().string();
            Addon addon;
            addon.name = addon_name;

            std::ifstream file(entry.path());
            std::string line;
            std::string current_section = "";

            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;

                if (line[0] == '[' && line.back() == ']') {
                    current_section = line.substr(1, line.size() - 2);
                    continue;
                }

                auto eq_pos = line.find('=');
                if (eq_pos != std::string::npos) {
                    std::string key = line.substr(0, eq_pos);
                    std::string val = line.substr(eq_pos + 1);

                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    val.erase(0, val.find_first_not_of(" \t"));
                    val.erase(val.find_last_not_of(" \t") + 1);

                    val = parse_ansi_colors(val);

                    if (current_section == "Theme") {
                        if (key == "prompt_format") addon.prompt_format = val;
                        else if (key == "welcome_message") addon.welcome_message = val;
                    } else if (current_section == "Hooks") {
                        addon.hooks[key] = val;
                    } else {
                        addon.commands[key] = val;
                    }
                }
            }
            loaded_addons[addon_name] = addon;
        }
    }
}

bool AddonManager::enable_addon(const std::string& name) {
    if (loaded_addons.find(name) != loaded_addons.end()) {
        active_addon_name = name;
        save_active_config();
        return true;
    }
    return false;
}

void AddonManager::disable_addon() {
    active_addon_name = "";
    save_active_config();
}

bool AddonManager::is_addon_active() const {
    return !active_addon_name.empty() && loaded_addons.find(active_addon_name) != loaded_addons.end();
}

std::string AddonManager::get_active_addon_name() const { return active_addon_name; }

void AddonManager::save_active_config() {
    std::ofstream file(config_file);
    if (file.is_open()) file << active_addon_name;
}

void AddonManager::load_active_config() {
    std::ifstream file(config_file);
    if (file.is_open()) std::getline(file, active_addon_name);
}

std::string AddonManager::get_prompt(const std::string& current_folder) const {
    if (!is_addon_active()) return "";
    const auto& addon = loaded_addons.at(active_addon_name);
    if (addon.prompt_format.empty()) return "";

    std::string prompt = addon.prompt_format;
    size_t pos = prompt.find("%folder%");
    if (pos != std::string::npos) prompt.replace(pos, 8, current_folder);
    return prompt;
}

std::string AddonManager::get_welcome_message() const {
    if (!is_addon_active()) return "";
    return loaded_addons.at(active_addon_name).welcome_message;
}

void AddonManager::trigger_event(const std::string& event_name) {
    if (!is_addon_active()) return;
    const auto& hooks = loaded_addons.at(active_addon_name).hooks;
    auto it = hooks.find(event_name);
    if (it != hooks.end()) std::system(it->second.c_str());
}

bool AddonManager::has_addon_command(const std::string& phrase) const {
    if (!is_addon_active()) return false;
    const auto& cmds = loaded_addons.at(active_addon_name).commands;
    return cmds.find(phrase) != cmds.end();
}

std::string AddonManager::get_addon_command(const std::string& phrase) const {
    if (!is_addon_active()) return "";
    const auto& cmds = loaded_addons.at(active_addon_name).commands;
    auto it = cmds.find(phrase);
    return (it != cmds.end()) ? it->second : "";
}

void AddonManager::print_addons_list() const {
    std::cout << Color::CYAN << "\n--- Available Addons ---\n" << Color::RESET;
    if (loaded_addons.empty()) {
        std::cout << "  No addons found in addons folder.\n";
    } else {
        for (const auto& [name, addon] : loaded_addons) {
            bool isActive = (name == active_addon_name);
            std::cout << "  " << (isActive ? Color::GREEN + "[Active]   " : Color::GRAY + "[Disabled] ") 
                      << Color::RESET << name << "\n";
        }
    }
    std::cout << Color::CYAN << "------------------------\n";
    std::cout << Color::GRAY << "Commands:\n  addons enable <name>\n  addons disable\n\n" << Color::RESET;
}
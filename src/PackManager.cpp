#include "PackManager.hpp"
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

PackManager::PackManager() {
    std::string base_dir = get_base_dir();
    packs_dir = base_dir + "/packs";
    config_file = base_dir + "/active_packs";

    try { fs::create_directories(packs_dir); } catch (...) {}

    scan_packs();
    load_active_packs();
}

void PackManager::scan_packs() {
    packs.clear();
    if (!fs::exists(packs_dir)) return;

    for (const auto& entry : fs::directory_iterator(packs_dir)) {
        if (entry.path().extension() == ".plainc") {
            std::string pack_id = entry.path().stem().string();
            CommandPack pack;
            pack.name = pack_id;

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

                    if (current_section == "info") {
                        if (key == "description") pack.description = val;
                    } else {
                        auto pipe_pos = val.find('|');
                        PackCommand pcmd;
                        if (pipe_pos != std::string::npos) {
                            pcmd.sys_cmd = val.substr(0, pipe_pos);
                            pcmd.description = val.substr(pipe_pos + 1);

                            pcmd.sys_cmd.erase(pcmd.sys_cmd.find_last_not_of(" \t") + 1);
                            pcmd.description.erase(0, pcmd.description.find_first_not_of(" \t"));
                        } else {
                            pcmd.sys_cmd = val;
                            pcmd.description = "Custom command";
                        }
                        pack.commands[key] = pcmd;
                    }
                }
            }
            packs[pack_id] = pack;
        }
    }
}

bool PackManager::enable_pack(const std::string& name) {
    if (packs.find(name) != packs.end()) {
        packs[name].is_enabled = true;
        save_active_packs();
        return true;
    }
    return false;
}

bool PackManager::disable_pack(const std::string& name) {
    if (packs.find(name) != packs.end()) {
        packs[name].is_enabled = false;
        save_active_packs();
        return true;
    }
    return false;
}

void PackManager::save_active_packs() {
    std::ofstream file(config_file);
    if (file.is_open()) {
        for (const auto& [id, pack] : packs) {
            if (pack.is_enabled) file << id << "\n";
        }
    }
}

void PackManager::load_active_packs() {
    std::ifstream file(config_file);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (packs.find(line) != packs.end()) packs[line].is_enabled = true;
    }
}

bool PackManager::has_command(const std::string& phrase) const {
    for (const auto& [id, pack] : packs) {
        if (pack.is_enabled && pack.commands.find(phrase) != pack.commands.end()) return true;
    }
    return false;
}

std::string PackManager::get_command(const std::string& phrase) const {
    for (const auto& [id, pack] : packs) {
        if (pack.is_enabled) {
            auto it = pack.commands.find(phrase);
            if (it != pack.commands.end()) return it->second.sys_cmd;
        }
    }
    return "";
}

void PackManager::print_packs_list() const {
    std::cout << Color::CYAN << "\n--- Command Packs (.plainc) ---\n" << Color::RESET;
    if (packs.empty()) {
        std::cout << "  No packs found in packs folder.\n";
    } else {
        for (const auto& [id, pack] : packs) {
            std::cout << "  " << (pack.is_enabled ? Color::GREEN + "[Active]   " : Color::GRAY + "[Disabled] ") 
                      << Color::RESET << id << Color::GRAY << " - " << pack.description << Color::RESET << "\n";
        }
    }
    std::cout << Color::CYAN << "--------------------------------\n";
    std::cout << Color::GRAY << "Commands:\n  packs enable <name>\n  packs disable <name>\n\n" << Color::RESET;
}

void PackManager::print_active_pack_commands() const {
    for (const auto& [id, pack] : packs) {
        if (pack.is_enabled) {
            std::cout << Color::CYAN << "\n--- Pack: " << id << " ---\n" << Color::RESET;
            for (const auto& [cmd, pcmd] : pack.commands) {
                std::cout << Color::GREEN << "  " << cmd;
                if (cmd.length() < 18) std::cout << std::string(18 - cmd.length(), ' ');
                std::cout << Color::RESET << "- " << pcmd.description << "\n";
            }
        }
    }
}
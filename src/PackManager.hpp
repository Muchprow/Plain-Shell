#pragma once
#include <string>
#include <unordered_map>

struct PackCommand {
    std::string sys_cmd;
    std::string description;
};

struct CommandPack {
    std::string name;
    std::string description;
    bool is_enabled = false;
    std::unordered_map<std::string, PackCommand> commands;
};

class PackManager {
public:
    PackManager();
    void scan_packs();
    bool enable_pack(const std::string& name);
    bool disable_pack(const std::string& name);

    bool has_command(const std::string& phrase) const;
    std::string get_command(const std::string& phrase) const;

    void print_packs_list() const;
    void print_active_pack_commands() const;

private:
    std::string packs_dir;
    std::string config_file;
    std::unordered_map<std::string, CommandPack> packs;

    void save_active_packs();
    void load_active_packs();
};
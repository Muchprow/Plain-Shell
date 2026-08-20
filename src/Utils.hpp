#pragma once
#include <string>
#include <filesystem>
#include <cstdlib>

inline std::string get_base_dir() {
    if (std::filesystem::exists("./portable")) {
        return "./portable";
    }
    #if defined(_WIN32)
        const char* home = std::getenv("USERPROFILE");
    #else
        const char* home = std::getenv("HOME");
    #endif
    return home ? std::string(home) + "/.plain" : "./.plain";
}
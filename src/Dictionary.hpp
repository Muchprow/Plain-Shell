#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class Dictionary {
public:
    Dictionary();
    
    std::string translate(const std::vector<std::string>& tokens) const;
    
    void print_help() const;

private:
    std::unordered_map<std::string, std::string> win_map;
    std::unordered_map<std::string, std::string> unix_map;
};

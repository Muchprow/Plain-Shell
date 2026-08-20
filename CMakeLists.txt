cmake_minimum_required(VERSION 3.15)
project(Plain VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Plain 
    src/main.cpp 
    src/Dictionary.cpp 
    src/Shell.cpp
    src/AddonManager.cpp
    src/HistoryManager.cpp
    src/PackManager.cpp
)

if(MINGW)
    target_link_options(Plain PRIVATE -static -static-libgcc -static-libstdc++)
endif()
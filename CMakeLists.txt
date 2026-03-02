cmake_minimum_required(VERSION 3.10.0)
project(Untitled_Game VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Untitled_Game main.cpp)

find_path(RL_INCLUDE_DIR NAMES raylib.h PATHS /opt/homebrew/opt/raylib/include)
include_directories(${RL_INCLUDE_DIR})

find_library(RL_LIBRARY NAMES raylib PATHS /opt/homebrew/opt/raylib/lib)

target_link_libraries(Untitled_Game ${RL_LIBRARY})
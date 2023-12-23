cmake_minimum_required(VERSION 3.15)

project(gravSim)

set(CMAKE_CXX_STANDARD 14)

add_compile_definitions(ENABLE_LOGS)

add_executable(gravSim main.cpp)

target_link_libraries(gravSim GL glfw)

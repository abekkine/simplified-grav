cmake_minimum_required(VERSION 3.15)

project(gravSim)

set(CMAKE_CXX_STANDARD 14)

add_compile_definitions(ENABLE_LOGS)

if (UNIX)
    # Nothing to do
else ()
    include_directories($ENV{EXT_LIB_DIR}/glew-2.1.0/include)
    include_directories($ENV{EXT_LIB_DIR}/glfw-3.3.2/include)
    link_directories($ENV{EXT_LIB_DIR}/glfw-3.3.2/lib-vc2019)
endif ()

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_executable(gravSim main.cpp)

if (UNIX)
    target_link_libraries(gravSim GL glfw)
else ()
    target_link_libraries(gravSim glfw3 opengl32)
endif ()

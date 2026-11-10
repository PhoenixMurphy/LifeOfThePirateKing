cmake_minimum_required(VERSION 3.22)
project(RaylibGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# compile_commands.json lets VS Code's C/C++ extension find raylib.h
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Executables go in build/bin on every platform/generator
# ($<1:...> stops Visual Studio adding a Debug/Release subfolder).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin>)

# ---------------------------------------------------------------------------
# raylib (vendored in external/raylib) - no internet needed
# ---------------------------------------------------------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
add_subdirectory(external/raylib EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# containers: our custom templates in src/containers/
# (CircularList.h, SinglyLinkedList.h).
# A header-only (INTERFACE) library: templates have nothing to compile on
# their own - they're compiled inside whatever includes them. Linking to it
# gives a target the src/ include path, so it can write
#   #include "containers/CircularList.h"
# ---------------------------------------------------------------------------
add_library(containers INTERFACE)
target_include_directories(containers INTERFACE ${CMAKE_SOURCE_DIR}/src)

# ---------------------------------------------------------------------------
# The game. Every .cpp under src/ is included automatically.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(Game ${GAME_SOURCES})
target_include_directories(Game PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(Game PRIVATE raylib containers)

if(MSVC)
    target_compile_options(Game PRIVATE /W4 /permissive-)
else()
    target_compile_options(Game PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy assets/ next to the executable after every build
add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:Game>/assets)

if(WIN32)
    # MinGW: link the C++ runtime statically so the .exe runs on any PC
    # without extra DLLs next to it.
    if(MINGW)
        target_link_options(Game PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
    # Visual Studio: make Game the start-up project, run it from build/bin.
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)
        set_target_properties(Game PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Game>)
    endif()
endif()
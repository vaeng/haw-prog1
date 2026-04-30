# 3.25+ keeps preset and modern target behavior consistent on all student machines.
cmake_minimum_required(VERSION 3.25)
project(MyGame VERSION 0.1.0 LANGUAGES CXX)

# Optional quality gates: keep strictness opt-in for local development.
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(ENABLE_CLANG_TIDY "Run clang-tidy during build when available" ON)

# Needed by clangd for reliable IntelliSense/navigation in VS Code.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enables `ctest` integration now and keeps test setup ready as the project grows.
include(CTest)

find_package(SFML 3 COMPONENTS System Window Graphics Audio REQUIRED)

add_executable(game)

# New .cpp files under src/ are picked up automatically so they inherit target compile/include settings.
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# New headers under include/ are picked up automatically for IDE visibility and source grouping.
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx"
)

target_sources(game PRIVATE ${GAME_SOURCES} ${GAME_HEADERS})
target_include_directories(game PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Prefer target-level requirements over global CMAKE_CXX_STANDARD.
target_compile_features(game PRIVATE cxx_std_20)

if(MSVC)
    # clang-cl uses MSVC-style flags on Windows.
    target_compile_options(game PRIVATE /W4 /permissive-)
    if(ENABLE_WARNINGS_AS_ERRORS)
        target_compile_options(game PRIVATE /WX)
    endif()
else()
    # GCC/Clang warning baseline on Linux/macOS.
    target_compile_options(game PRIVATE -Wall -Wextra -Wpedantic)
    if(ENABLE_WARNINGS_AS_ERRORS)
        target_compile_options(game PRIVATE -Werror)
    endif()
endif()

target_link_libraries(game
    PRIVATE
    SFML::System
    SFML::Window
    SFML::Graphics
    SFML::Audio
)

if(ENABLE_CLANG_TIDY)
    # Tool remains optional: configure succeeds even if clang-tidy is not installed.
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set_target_properties(game PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()
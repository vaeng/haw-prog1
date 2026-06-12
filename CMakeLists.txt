cmake_minimum_required(VERSION 3.22)

# ------------------------------------------------------------
# Project metadata
# ------------------------------------------------------------
project(
    MyGame
    VERSION 0.1
    LANGUAGES CXX
)

# ------------------------------------------------------------
# Tooling support
# ------------------------------------------------------------
# Required for clangd / clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------
# Engine logic library
# ------------------------------------------------------------
add_library(engine
    src/engine/Core.cpp
    src/engine/GameObject.cpp
    src/engine/Vector2.cpp
    src/engine/RenderComponent.cpp
)

target_compile_features(engine PUBLIC cxx_std_23)

# Public headers propagate include paths to dependents
target_include_directories(engine
    PUBLIC
        include
)

# engine uses SFML types in its headers/sources; link SFML to ensure include
# directories and usage requirements are available when compiling engine.
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)
target_link_libraries(engine
    PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)

# ------------------------------------------------------------
# Main executable
# ------------------------------------------------------------
add_executable(my_game
    src/main.cpp
)

# ------------------------------------------------------------
# SFML 3.0 (vcpkg)
# ------------------------------------------------------------
target_link_libraries(my_game
    PRIVATE
        engine
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
)

# ------------------------------------------------------------
# Assets: copy next to executable for dev runs
# ------------------------------------------------------------
add_custom_command(
    TARGET my_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:my_game>/assets
    COMMENT "Copying assets to build output"
)

# ------------------------------------------------------------
# Source discovery for tooling (NOT for build)
# Safe due to CONFIGURE_DEPENDS
# ------------------------------------------------------------
file(GLOB_RECURSE FORMAT_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/game/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/*.cpp
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/include/*.h
)

file(GLOB_RECURSE TIDY_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/game/*.cpp
    ${CMAKE_SOURCE_DIR}/src/engine/*.cpp
)

# ------------------------------------------------------------
# clang-format targets
# ------------------------------------------------------------
find_program(CLANG_FORMAT_EXE clang-format)

if (CLANG_FORMAT_EXE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE}
            -i
            --style=file
            ${FORMAT_FILES}
        COMMENT "Formatting source files (clang-format)"
    )

    add_custom_target(check-format
        COMMAND ${CLANG_FORMAT_EXE}
            --dry-run
            --Werror
            --style=file
            ${FORMAT_FILES}
        COMMENT "Checking code formatting (CI)"
    )
endif()

# ------------------------------------------------------------
# clang-tidy targets
# ------------------------------------------------------------
find_program(CLANG_TIDY_EXE clang-tidy)

if (CLANG_TIDY_EXE)
    add_custom_target(clang-tidy
        COMMAND ${CLANG_TIDY_EXE}
            -p ${CMAKE_BINARY_DIR}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
            ${TIDY_FILES}
        COMMENT "Running clang-tidy"
    )

    add_custom_target(check-tidy
        COMMAND ${CLANG_TIDY_EXE}
            -p ${CMAKE_BINARY_DIR}
            --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy
            --warnings-as-errors=*
            ${TIDY_FILES}
        COMMENT "Checking clang-tidy (CI)"
    )
endif()

# ------------------------------------------------------------
# Optional install rules (future-proofing)
# ------------------------------------------------------------
install(TARGETS my_game
    RUNTIME DESTINATION bin
)

install(DIRECTORY assets/
    DESTINATION bin/assets
)
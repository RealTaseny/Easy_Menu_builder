cmake_minimum_required(VERSION 3.22.1)

project(menu_emulator_lib VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_ROOT ../..)          # ../../

set(MENU_NAVIGATOR_SOURCE_DIR  ${PROJECT_ROOT}/lib/menu_navigator)
set(MENU_NAVIGATOR_BINARY_DIR  ${CMAKE_BINARY_DIR}/menu_navigator_build)

add_subdirectory(${MENU_NAVIGATOR_SOURCE_DIR} ${MENU_NAVIGATOR_BINARY_DIR})

add_library(menu_emulator_lib SHARED
        src/generated_code.cpp
)

target_include_directories(menu_emulator_lib PUBLIC
        include
        ${MENU_NAVIGATOR_SOURCE_DIR}/include
)

target_link_libraries(menu_emulator_lib PUBLIC
        menu_navigator_lib
)

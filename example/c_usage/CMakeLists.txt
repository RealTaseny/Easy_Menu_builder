cmake_minimum_required(VERSION 3.22.1)

project(c_usage VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_ROOT ../..)

set(MENU_NAVIGATOR_SOURCE_DIR ${PROJECT_ROOT}/lib/menu_navigator)
set(MENU_NAVIGATOR_BINARY_DIR ${CMAKE_BINARY_DIR}/menu_navigator_build)

add_subdirectory(${MENU_NAVIGATOR_SOURCE_DIR} ${MENU_NAVIGATOR_BINARY_DIR})

add_executable(c_usage
        main.c
        src/generated_code.cpp
)

target_include_directories(c_usage PRIVATE
        include
)

target_link_libraries(c_usage PRIVATE
        menu_navigator_lib
)

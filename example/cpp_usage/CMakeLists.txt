cmake_minimum_required(VERSION 3.22.1)

project(cpp_usage VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_ROOT ../..)

set(MENU_NAVIGATOR_SOURCE_DIR ${PROJECT_ROOT}/lib/menu_navigator)
set(MENU_NAVIGATOR_BINARY_DIR ${CMAKE_BINARY_DIR}/menu_navigator_build)

add_subdirectory(${MENU_NAVIGATOR_SOURCE_DIR} ${MENU_NAVIGATOR_BINARY_DIR})

add_executable(cpp_usage
        main.cpp
        src/generated_code.cpp
)

target_include_directories(cpp_usage PRIVATE
        include
)

target_link_libraries(cpp_usage PRIVATE
        menu_navigator_lib
)

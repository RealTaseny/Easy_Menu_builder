cmake_minimum_required(VERSION 3.16)

project(Menu_builder VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置自动生成moc文件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        src/main_window.cpp
        src/menu.cpp
        src/cpp_playground.cpp
        src/menu_builder_window.cpp
        src/menu_item_editor.cpp
        src/OLED_simulator.cpp
        src/code_generator.cpp
        src/menu_c_wrapper.cpp
)

set(PROJECT_HEADERS
        include/code_generator.h
        include/main_window.h
        include/menu.h
        include/cpp_playground.h
        include/menu_builder_window.h
        include/menu_item_editor.h
        include/OLED_simulator.h
        src/menu_c_wrapper.cpp
        include/menu_c_wrapper.h
)

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
)


target_include_directories(${PROJECT_NAME} PUBLIC
        include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

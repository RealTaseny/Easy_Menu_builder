//
// Created by taseny on 25-7-21.
//

// main.cpp
#include <QApplication>

#include "oled_window.h"
#include "menu_builder_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MenuBuilderWindow menu_builder_window;
    OledWindow oled_window(&menu_builder_window, &menu_builder_window);

    oled_window.show();
    menu_builder_window.show();

    return app.exec();
}

//
// Created by taseny on 25-7-21.
//

// main.cpp
#include <QApplication>

#include "oled_window.h"
#include "menu_builder_window.h"

QString appDir = "";

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    appDir = QCoreApplication::applicationDirPath();

    MenuBuilderWindow menu_builder_window;

    OledWindow *oled_window;

    if (argc == 4 && atoi(argv[1]) > 0 && atoi(argv[2]) > 0 && atoi(argv[3]) > 0)
    {
    	oled_window = new OledWindow(nullptr, &menu_builder_window, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]) );
    }
    else
        oled_window = new OledWindow(nullptr, &menu_builder_window);

    oled_window->show();
    menu_builder_window.show();

    return app.exec();
}

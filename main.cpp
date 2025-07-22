//
// Created by taseny on 25-7-21.
//

// main.cpp
#include <QApplication>

#include "main_window.h"
#include "menu_builder_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    MenuBuilderWindow menuBuilderWindow;
    mainWindow.show();
    menuBuilderWindow.show();

    return app.exec();
}

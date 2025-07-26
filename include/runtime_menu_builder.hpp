//
// Created by taseny on 25-7-26.
//

#ifndef RUNTIME_MENU_BUILDER_HPP
#define RUNTIME_MENU_BUILDER_HPP

#include "menu_navigator.h"

#include <QProcess>
#include <QDebug>
#include <QLibrary>

class RuntimeMenuBuilder
{
public:
    static Menu::Navigator* buildMenu(const QList<MenuItemEditor::ItemData>& items)
    {
        QString buildDir = "/tmp/menu_build";
        QString soPath = buildDir + "/libmenu_emulator_lib.so";
        QString projectDir = "/home/taseny/Easy_Menu_Builder/lib/menu_emulator_lib";

        QDir().mkpath(buildDir);
        QFile::remove(soPath);

        QProcess cmake;
        cmake.setWorkingDirectory(buildDir);
        cmake.start("cmake", QStringList() << projectDir);
        cmake.waitForFinished(-1);
        qDebug() << "CMake Output:\n" << cmake.readAllStandardOutput();
        qDebug() << "CMake Error:\n" << cmake.readAllStandardError();

        QProcess make;
        make.setWorkingDirectory(buildDir);
        make.start("make");
        make.waitForFinished(-1);
        qDebug() << "Make Output:\n" << make.readAllStandardOutput();
        qDebug() << "Make Error:\n" << make.readAllStandardError();

        static QLibrary menuLib(soPath);
        if (menuLib.isLoaded())
            menuLib.unload();

        menuLib.setFileName(soPath);
        if (menuLib.load())
        {
            typedef Menu::Navigator* (*getNavigator)();
            getNavigator get_navigator = (getNavigator)menuLib.resolve("getNavigator");

            if (get_navigator)
            {
                return get_navigator();
            }
            return new Menu::Navigator(nullptr);
        }
    }
};

#endif //RUNTIME_MENU_BUILDER_HPP

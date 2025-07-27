//
// Created by taseny on 25-7-26.
//

#ifndef RUNTIME_MENU_BUILDER_HPP
#define RUNTIME_MENU_BUILDER_HPP

#include "menu_navigator.h"

#include <QProcess>
#include <QDebug>
#include <QLibrary>
#include <QtGlobal>
#include <QApplication>

extern QString appDir;

class RuntimeMenuBuilder
{
public:
        static Menu::Navigator* buildMenu()
        {
        	static QLibrary menuLib;
            QString buildDir;
            QString libFile;
            QString projectDir;

        	projectDir = appDir + "/../lib/menu_emulator_lib";

        	buildDir = projectDir + "/build";

#if defined(Q_OS_WIN)
            libFile = buildDir + "/libmenu_emulator_lib.dll";
#elif defined(Q_OS_LINUX)
            libFile = buildDir + "/libmenu_emulator_lib.so";
#endif

        	if (menuLib.isLoaded())
        		menuLib.unload();

        	QDir().rmdir(buildDir);
        	QDir().mkpath(buildDir);

            QFile::remove(libFile);

            QProcess cmake;
            cmake.setWorkingDirectory(buildDir);

#if defined(Q_OS_WIN)
        	cmake.start("cmake", QStringList() << projectDir << "-G" << "MinGW Makefiles" << "-DCMAKE_BUILD_TYPE=Release" );
#elif defined(Q_OS_LINUX)
        	cmake.start("cmake", QStringList() << projectDir << "-DCMAKE_BUILD_TYPE=Release");
#endif

        	cmake.waitForFinished(-1);

            QProcess build;
            build.setWorkingDirectory(buildDir);

#if defined(Q_OS_WIN)
            build.start("cmake", QStringList() << "--build" << ".");
#else
            build.start("make");
#endif

           build.waitForFinished(-1);

            menuLib.setFileName(libFile);
            if (menuLib.load())
            {
                typedef Menu::Navigator* (*getNavigator)();
                getNavigator get_navigator = (getNavigator)menuLib.resolve("getNavigator");

                if (get_navigator)
                    return get_navigator();
                else
                    qWarning() << "Failed to resolve symbol: getNavigator";
            } else {
                qWarning() << "Failed to load dynamic lib:" << libFile;
            }

            return new Menu::Navigator(nullptr);
        }

};

#endif //RUNTIME_MENU_BUILDER_HPP

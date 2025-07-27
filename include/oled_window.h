//
// Created by taseny on 25-7-21.
//

#ifndef OLED_WINDOW_H
#define OLED_WINDOW_H

#include "oled_simulator.h"
#include "menu_item_editor.h"
#include "menu_builder_window.h"

class OledWindow : public QMainWindow
{
    Q_OBJECT

public:
    OledWindow(QWidget* parent = nullptr, MenuBuilderWindow* menuBuilderWindow = nullptr, int width = 128, int height = 64, int pixelSize = 2);
    ~OledWindow() = default;

private slots:
    void updateMenu() const;

private:
    MenuBuilderWindow* m_menuBuilderWindow;
    OledSimulator* m_oled;
};


#endif //OLED_WINDOW_H

//
// Created by taseny on 25-7-21.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "OLED_simulator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    OLEDSimulator* m_oled;
};


#endif //MAIN_WINDOW_HPP

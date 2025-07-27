//
// Created by taseny on 25-7-21.
//

#include "oled_window.h"
#include "menu_builder_window.h"
#include "menu_navigator.h"
#include "runtime_menu_builder.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>

Menu::Navigator* navigator = new Menu::Navigator(nullptr);

OledWindow::OledWindow(QWidget* parent, MenuBuilderWindow* menuBuilderWindow)
    : QMainWindow(parent)
{
    m_menuBuilderWindow = menuBuilderWindow;

    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto mainLayout = new QVBoxLayout(centralWidget);

    m_oled = new OledSimulator(this);
    mainLayout->addWidget(m_oled);

    auto buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonLayout);

    auto buttonContainer = new QWidget();
    auto buttonGridLayout = new QGridLayout(buttonContainer);

    auto upButton = new QPushButton("↑");
    auto downButton = new QPushButton("↓");
    auto leftButton = new QPushButton("←");
    auto rightButton = new QPushButton("→");

    buttonGridLayout->addWidget(upButton, 0, 1);
    buttonGridLayout->addWidget(leftButton, 1, 0);
    buttonGridLayout->addWidget(rightButton, 1, 2);
    buttonGridLayout->addWidget(downButton, 2, 1);

    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonContainer);
    buttonLayout->addStretch();

    connect(upButton, &QPushButton::clicked, this, [this]()
    {
        navigator->handleInput(Menu::keyValue::UP);
    });

    connect(downButton, &QPushButton::clicked, this, [this]()
    {
        navigator->handleInput(Menu::keyValue::DOWN);
    });

    connect(leftButton, &QPushButton::clicked, this, [this]()
    {
        navigator->handleInput(Menu::keyValue::LEFT);
    });

    connect(rightButton, &QPushButton::clicked, this, [this]()
    {
        navigator->handleInput(Menu::keyValue::RIGHT);
    });

    connect(this->m_menuBuilderWindow, &MenuBuilderWindow::menuStructureUpdated,
            this, &OledWindow::updateMenu);

    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]()
    {
        static char* display_buffer;

        if (navigator == nullptr)
            return;

        navigator->refreshDisplay();

        display_buffer = navigator->getDisplayBuffer();

        m_oled->clear();

        for (uint8_t i = 0; i < MAX_DISPLAY_ITEM; ++i)
        {
            m_oled->drawString(0, i * 12, QString::fromUtf8(display_buffer + i * MAX_DISPLAY_CHAR));
        }
    });
    timer->start(50);
}

void OledWindow::updateMenu() const
{
    if (navigator)
    {
        delete navigator;
        navigator = nullptr;
    }

    navigator = RuntimeMenuBuilder::buildMenu();

    m_oled->clear();
}

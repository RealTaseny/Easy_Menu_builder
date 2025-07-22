//
// Created by taseny on 25-7-21.
//

#include "main_window.h"
#include "cpp_playground.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // 创建中心窗口
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 创建主布局
    auto mainLayout = new QVBoxLayout(centralWidget);

    // 创建OLED模拟器
    m_oled = new OLEDSimulator(this);
    mainLayout->addWidget(m_oled);

    // 创建按钮布局
    auto buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonLayout);

    // 创建按钮容器
    auto buttonContainer = new QWidget();
    auto buttonGridLayout = new QGridLayout(buttonContainer);

    // 创建四个方向按钮
    auto upButton = new QPushButton("↑");
    auto downButton = new QPushButton("↓");
    auto leftButton = new QPushButton("←");
    auto rightButton = new QPushButton("→");

    // 设置按钮布局
    buttonGridLayout->addWidget(upButton, 0, 1);
    buttonGridLayout->addWidget(leftButton, 1, 0);
    buttonGridLayout->addWidget(rightButton, 1, 2);
    buttonGridLayout->addWidget(downButton, 2, 1);

    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonContainer);
    buttonLayout->addStretch();

    // 连接按钮信号
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


    // 设置定时器更新显示
    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]()
    {
        static char* display_buffer;

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

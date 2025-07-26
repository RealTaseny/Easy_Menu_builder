//
// Created by taseny on 25-7-21.
//

#include "oled_simulator.h"

#include <QPainter>
#include <QFont>

OledSimulator::OledSimulator(QWidget* parent)
    : QWidget(parent)
      , m_width(128)
      , m_height(64)
      , m_pixelSize(2)
{
    setResolution(m_width, m_height);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    setFixedSize(m_width * m_pixelSize, m_height * m_pixelSize);
}

void OledSimulator::setResolution(const int width, const int height)
{
    m_width = width;
    m_height = height;

    m_pixels.resize(height);
    for (auto& row : m_pixels)
    {
        row.resize(width, false);
    }

    setFixedSize(width * m_pixelSize, height * m_pixelSize);
    update();
}

void OledSimulator::setPixel(const int x, const int y, const bool on)
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_pixels[y][x] = on;
        update();
    }
}

void OledSimulator::clear()
{
    for (auto& row : m_pixels)
    {
        std::fill(row.begin(), row.end(), false);
    }
    update();
}

void OledSimulator::drawHLine(const int x, const int y, const int width)
{
    for (int i = 0; i < width && (x + i) < m_width; ++i)
    {
        setPixel(x + i, y, true);
    }
}

void OledSimulator::drawVLine(const int x, const int y, const int height)
{
    for (int i = 0; i < height && (y + i) < m_height; ++i)
    {
        setPixel(x, y + i, true);
    }
}

void OledSimulator::drawRect(const int x, const int y, const int width, const int height, const bool fill)
{
    if (fill)
    {
        for (int dy = 0; dy < height; ++dy)
        {
            for (int dx = 0; dx < width; ++dx)
            {
                setPixel(x + dx, y + dy, true);
            }
        }
    }
    else
    {
        drawHLine(x, y, width);
        drawHLine(x, y + height - 1, width);
        drawVLine(x, y, height);
        drawVLine(x + width - 1, y, height);
    }
}

void OledSimulator::drawString(const int x, const int y, const QString& str)
{
    QImage img(m_width, m_height, QImage::Format_Mono);
    img.fill(0);

    QPainter painter(&img);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier", 8));
    painter.drawText(x, y + 12, str);

    for (int py = 0; py < m_height; ++py)
    {
        for (int px = 0; px < m_width; ++px)
        {
            if (img.pixelColor(px, py) == Qt::white)
            {
                setPixel(px, py, true);
            }
        }
    }
}

void OledSimulator::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            QRect pixelRect(x * m_pixelSize,
                            y * m_pixelSize,
                            m_pixelSize,
                            m_pixelSize);

            painter.fillRect(pixelRect,
                             m_pixels[y][x] ? Qt::white : Qt::black);
        }
    }
}

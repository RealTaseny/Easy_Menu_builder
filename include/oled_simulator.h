//
// Created by taseny on 25-7-21.
//

#ifndef MENU_BUILDER_OLEDSIMULATOR_H
#define MENU_BUILDER_OLEDSIMULATOR_H

#include <QWidget>
#include <vector>

class OledSimulator : public QWidget
{
    Q_OBJECT

public:
    explicit OledSimulator(QWidget* parent = nullptr);

    // 设置分辨率
    void setResolution(int width, int height);

    // 基础绘图API
    void setPixel(int x, int y, bool on = true);
    void clear();
    void drawChar(int x, int y, char ch);
    void drawString(int x, int y, const QString& str);
    void drawRect(int x, int y, int width, int height, bool fill = false);
    void drawHLine(int x, int y, int width);
    void drawVLine(int x, int y, int height);

    // 获取显示缓冲
    const std::vector<std::vector<bool>>& buffer() const { return m_pixels; }

    // 获取分辨率
    int width() const { return m_width; }
    int height() const { return m_height; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<std::vector<bool>> m_pixels; // 像素矩阵
    int m_width; // 显示器宽度(像素)
    int m_height; // 显示器高度(像素)
    int m_pixelSize; // 单个像素的显示大小(用于放大显示)
};

#endif //MENU_BUILDER_OLEDSIMULATOR_H

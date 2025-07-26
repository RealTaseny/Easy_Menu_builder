#include "generated_header.h"

#include <iostream>

#define MCU_GPIO_0 1U
#define MCU_GPIO_1 2U
#define MCU_GPIO_2 3U
#define MCU_GPIO_3 4U

class HAL
{
public:
    HAL(const std::string& platform_name)
    {
        platform_name_ = platform_name;
    }

    /// 设置GPIO电平
    static bool get_gpio_level(const uint8_t pin)
    {
        return pin % 2 == 0;
        // 假设是获取GPIO电平的函数
    }

    /// 将GPIO设置为输入
    static bool gpio_set_input(uint8_t pin)
    {
        return true;
    }

    /// OLED显示接口
    static void oledDisplay(const char* str)
    {
        std::cout << str << std::endl;
    }

    /// 延时毫秒
    static void delay(uint32_t ms)
    {
        for (int i = 0; i < ms; i++)
        {
            for (int j = 0; j < 1000; j++);
        }
    }

private:
    std::string platform_name_;
};

class KeyScaner
{
public:
    KeyScaner() : key_buffer_(0), key_value_(Menu::keyValue::NONE)
    {
        HAL::gpio_set_input(MCU_GPIO_0);
        HAL::gpio_set_input(MCU_GPIO_1);
        HAL::gpio_set_input(MCU_GPIO_2);
        HAL::gpio_set_input(MCU_GPIO_3);
    }

    Menu::keyValue key_scaner()
    {
        key_buffer_ |= key_buffer_ << HAL::get_gpio_level(MCU_GPIO_0);
        key_buffer_ |= key_buffer_ << HAL::get_gpio_level(MCU_GPIO_1);
        key_buffer_ |= key_buffer_ << HAL::get_gpio_level(MCU_GPIO_2);
        key_buffer_ |= key_buffer_ << HAL::get_gpio_level(MCU_GPIO_3);

        switch (key_buffer_)
        {
        case 0x0001:
            key_value_ = Menu::keyValue::UP;
            break;
        case 0x0010:
            key_value_ = Menu::keyValue::DOWN;
            break;
        case 0x0100:
            key_value_ = Menu::keyValue::LEFT;
            break;
        case 0x1000:
            key_value_ = Menu::keyValue::RIGHT;
            break;
        }
        return key_value_;
    }

private:
    Menu::keyValue key_value_;
    uint16_t key_buffer_;
};

int main()
{
    const std::string platform_name = "ESP32";
    KeyScaner key_scaner;
    for (;;)
    {
        navigator->handleInput(key_scaner.key_scaner());
        navigator->refreshDisplay();
        char* ptrDisplayBuffer = navigator->getDisplayBuffer();
        HAL::oledDisplay(ptrDisplayBuffer);
        HAL::delay(100);
    }
    return 0;
}

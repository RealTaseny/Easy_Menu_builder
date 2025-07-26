#include "menu_c_wrapper.h"
#include "generated_header.h"

#include <stdint.h>
#include <stdio.h>

#define MCU_GPIO_0 1U
#define MCU_GPIO_1 2U
#define MCU_GPIO_2 3U
#define MCU_GPIO_3 4U


/// 设置GPIO电平
static uint8_t get_gpio_level(const uint8_t pin)
{
    return pin % 2 == 0;
    // 假设是获取GPIO电平的函数
}

/// 将GPIO设置为输入
static uint8_t gpio_set_input(uint8_t pin)
{
    return 1U;
}

/// OLED显示接口
static void oledDisplay(const char* str)
{
    printf("%s\n", str);
}

/// 延时毫秒
static void delay(uint32_t ms)
{
    for (int i = 0; i < ms; i++)
    {
        for (int j = 0; j < 1000; j++);
    }
}

void key_scaner_init()
{
    gpio_set_input(MCU_GPIO_0);
    gpio_set_input(MCU_GPIO_1);
    gpio_set_input(MCU_GPIO_2);
    gpio_set_input(MCU_GPIO_3);
}

uint8_t key_scaner()
{
    uint8_t key_value = 0;
    uint16_t key_buffer_ = 0;

    key_buffer_ |= key_buffer_ << get_gpio_level(MCU_GPIO_0);
    key_buffer_ |= key_buffer_ << get_gpio_level(MCU_GPIO_1);
    key_buffer_ |= key_buffer_ << get_gpio_level(MCU_GPIO_2);
    key_buffer_ |= key_buffer_ << get_gpio_level(MCU_GPIO_3);

    switch (key_buffer_)
    {
    case 0x0001:
        key_value = UP;
        break;
    case 0x0010:
        key_value = DOWN;
        break;
    case 0x0100:
        key_value = LEFT;
        break;
    case 0x1000:
        key_value = RIGHT;
        break;
    default:
        key_value = NONE;
    break;
    }
    return key_value;
}

void *navigator_;

int main()
{
    navigator_ = menu_builder(getMainItem());
    for (;;)
    {
        uint8_t key_value = key_scaner();

        menu_handle_input(navigator_, key_value);

        menu_refresh_display(navigator_);

        char *ptrDisplayBuffer = menu_get_display_buffer(navigator_);

        oledDisplay(ptrDisplayBuffer);

        delay(100);
    }
    menu_delete(navigator_);
    return 0;
}

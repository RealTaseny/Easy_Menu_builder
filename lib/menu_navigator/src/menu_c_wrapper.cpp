//
// Created by taseny on 25-7-22.
//

#include "menu_c_wrapper.h"

#ifdef __cplusplus
#include "menu_navigator.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* menu_builder(void* mainMenu)
{
    const auto menu = new Menu::Navigator(static_cast<Menu::menuItem*>(mainMenu));
    return menu;
}

void menu_delete(void* navigator)
{
    delete static_cast<Menu::Navigator*>(navigator);
}

void menu_handle_input(void* navigator, uint8_t key_value)
{
    const auto menu = static_cast<Menu::Navigator*>(navigator);
    switch (key_value)
    {
    case UP:
        menu->handleInput(Menu::keyValue::UP);
        break;
    case DOWN:
        menu->handleInput(Menu::keyValue::DOWN);
        break;
    case LEFT:
        menu->handleInput(Menu::keyValue::LEFT);
        break;
    case RIGHT:
        menu->handleInput(Menu::keyValue::RIGHT);
        break;
    default:
        menu->handleInput(Menu::keyValue::NONE);
    }
}

void menu_refresh_display(void* navigator)
{
    const auto menu = static_cast<Menu::Navigator*>(navigator);
    menu->refreshDisplay();
}

char* menu_get_display_buffer(void* navigator)
{
    const auto menu = static_cast<Menu::Navigator*>(navigator);
    return menu->getDisplayBuffer();
}

uint8_t menu_get_app_mode(void* navigator)
{
    const auto menu = static_cast<Menu::Navigator*>(navigator);
    return menu->getAppMode();
}

void menu_set_app_mode(void* navigator, uint8_t mode)
{
    const auto menu = static_cast<Menu::Navigator*>(navigator);
    menu->setAppMode(static_cast<bool>(mode));
}

#ifdef __cplusplus
}
#endif

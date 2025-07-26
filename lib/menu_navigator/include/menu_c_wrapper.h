//
// Created by taseny on 25-7-22.
//

#ifndef MENU_C_WRAPPER_H
#define MENU_C_WRAPPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE,
};


void* menu_builder(void* mainMenu);

void menu_delete(void* navigator);

void menu_handle_input(void* navigator, uint8_t key_value);

void menu_refresh_display(void* navigator);

char* menu_get_display_buffer(void* navigator);

uint8_t menu_get_app_mode(void* navigator);

void menu_set_app_mode(void* navigator, uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif //MENU_C_WRAPPER_H

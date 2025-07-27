#include "menu_navigator.h"
#include <cstdint>

using namespace Menu;

// 生成的变量定义
struct g_menu_variables_t
{
} g_menu_vars;


// 生成的回调函数和参数指针数组

// 生成的菜单项定义
menuItem* pidChildren[] = {
    menuItem::createNormalItem("KP", nullptr, 0),
};

menuItem* main_menuChildren[] = {
    menuItem::createNormalItem("PID", pidChildren, 1),
    menuItem::createNormalItem("Motor", nullptr, 0),
    menuItem::createNormalItem("New Item", nullptr, 0),
};


menuItem* mainMenu = menuItem::createNormalItem("Main Menu", main_menuChildren, 3);


 #ifdef __cplusplus
 
 extern "C" {
 #endif
 Navigator* getNavigator() 
 { 
 return new Navigator(mainMenu); 
 }
 #ifdef __cplusplus 
 }
 #endif


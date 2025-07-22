//
// Created by taseny on 25-7-21.
//

#include "cpp_playground.h"

using namespace Menu;

menuItem* mainMenu = menuItem::createNormalItem("Main Menu", nullptr, 0);

Navigator* navigator = new Navigator(mainMenu);

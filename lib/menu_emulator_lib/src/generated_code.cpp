#include "menu_navigator.h"
#include <cstdint>

using namespace Menu;

// 生成的变量定义
struct g_menu_variables_t
{
    bool g_set_camera_on = false;
    bool motor_state = false;
    float motor_kp = 0;
    float motor_ki = 0;
    float motor_kd = 0;
} g_menu_vars;


// 生成的回调函数和参数指针数组
void resetPID(void** args) {
    /*  */
}

void savePIDParameter(void** args) {
    /*  */
}

void run_car_task1(void** args) {
    /*  */
}

void run_car_task2(void** args) {
    /*  */
}

void run_car_task3(void** args) {
    /*  */
}

void allApp(void** args) {
    /*  */
}

void parameterApp(void** args) {
    /*  */
}

void savedstateApp(void** args) {
    /*  */
}

// resetPID 回调函数参数指针数组
void* pid_parameter_t[] = {
       nullptr // 请自行添加参数指针
};

// run_car_task1 回调函数参数指针数组
void* carTask_param_t[] = {
       nullptr // 请自行添加参数指针
};

//  回调函数参数指针数组
void* allAppArgs[] = {
       nullptr // 请自行添加参数指针
};

//  回调函数参数指针数组
void* parameterAppArgs[] = {
       nullptr // 请自行添加参数指针
};

//  回调函数参数指针数组
void* savedstateAppArgs[] = {
       nullptr // 请自行添加参数指针
};


// 生成的菜单项定义
menuItem* pidChildren[] = {
    menuItem::createChangeableItem<float>("KP", g_menu_vars.motor_kp, 0, 100, 1, nullptr),
    menuItem::createChangeableItem<float>("KI", g_menu_vars.motor_ki, 0, 10, 0.1, nullptr),
    menuItem::createChangeableItem<float>("KD", g_menu_vars.motor_kd, 0, 100, 1, nullptr),
    menuItem::createApp("Reset", pid_parameter_t, resetPID),
    menuItem::createApp("Save", pid_parameter_t, savePIDParameter),
};

menuItem* runcarChildren[] = {
    menuItem::createApp("Task1", carTask_param_t, run_car_task1),
    menuItem::createApp("Task2", carTask_param_t, run_car_task2),
    menuItem::createApp("Task3", carTask_param_t, run_car_task3),
};

menuItem* settingsChildren[] = {
    menuItem::createToggle("Camera", &g_menu_vars.g_set_camera_on, nullptr),
    menuItem::createToggle("MotorState", &g_menu_vars.motor_state, nullptr),
};

menuItem* restoreChildren[] = {
    menuItem::createApp("All", nullptr, nullptr),
    menuItem::createApp("Parameter", nullptr, nullptr),
    menuItem::createApp("SavedState", nullptr, nullptr),
};

menuItem* aboutChildren[] = {
    menuItem::createNormalItem("Version", nullptr, 0),
    menuItem::createNormalItem("SN", nullptr, 0),
};

menuItem* systemChildren[] = {
    menuItem::createNormalItem("Restore", restoreChildren, 3),
    menuItem::createNormalItem("About", aboutChildren, 2),
};

menuItem* main_menuChildren[] = {
    menuItem::createNormalItem("PID", pidChildren, 5),
    menuItem::createNormalItem("RunCar", runcarChildren, 3),
    menuItem::createNormalItem("Settings", settingsChildren, 2),
    menuItem::createNormalItem("System", systemChildren, 2),
};


menuItem* mainMenu = menuItem::createNormalItem("Main Menu", main_menuChildren, 4);


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


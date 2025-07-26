# Easy Menu Builder

> RELEASE VERSION V1.0

> 快速构建基于 OLED 显示器的菜单导航系统

---

## 项目简介

Key Features:
- **完全静态**的菜单, 效率高(无动画), 专为嵌入式系统打造
- 可自定义分辨率的`OLED`模拟器
- 利用`Qt`动态链接库`Hot Reload`技术实现生成代码**热重载**仿真
- 可保存配置工程, 便于多次修改
- 支持多种菜单项
  * 普通菜单项（Normal Item）
  * 可变参数项（Changeable Item）
  * 开关项（Toggle Item）
  * 应用项（Application Item）
- 可前台运行应用(应用项)
- 可绑定回调或者`lamda`函数(可变参数项/开关项)

---

## 项目结构

```shell
Easy_Menu_Builder
├── example   # 示例
│   ├── cpp_usage   # C++用法示例
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   └── generated_header.h
│   │   ├── main.cpp
│   │   ├── README.MD
│   │   └── src
│   │       └── generated_code.cpp
│   └── c_usage   # C用示例
│       ├── CMakeLists.txt
│       ├── include
│       │   └── generated_header.h
│       ├── main.c
│       ├── README.MD
│       └── src
│           └── generated_code.cpp
├── include   # 头文件
│   ├── code_generator.h
│   ├── menu_builder_window.h
│   ├── menu_item_editor.h
│   ├── oled_simulator.h
│   ├── oled_window.h
│   └── runtime_menu_builder.hpp
├── src   # 源文件
│   ├── code_generator.cpp
│   ├── menu_builder_window.cpp
│   ├── menu_item_editor.cpp
│   ├── oled_simulator.cpp
│   └── oled_window.cpp
├── lib   # 库
│   ├── menu_emulator_lib   # 热重载代码缓冲区
│   │   ├── CMakeLists.txt
│   │   ├── include
│   │   │   └── generated_code.h
│   │   └── src
│   │       └── generated_code.cpp
│   └── menu_navigator    # 静态菜单导航器源码(移植注意这个接口)
│       ├── CMakeLists.txt
│       ├── include
│       │   ├── menu_c_wrapper.h
│       │   └── menu_navigator.h
│       └── src
│           ├── menu_c_wrapper.cpp
│           └── menu_navigator.cpp
├── main.cpp
├── README.MD
└── CMakeLists.txt
```

---

## 使用方法

* 1. 新建一个工程
* 2. 在左边菜单编辑器编辑菜单项属性
* 3. 完成后可以保存工程以免配置丢失
* 4. 点击工具栏的`生成代码`按钮可以选择实时预览或生成C/C++代码

---

## 扩展说明

### 1.关于应用模式

此模式原来我是用在基于`FreeRTOS`的MCU上的，所以原来从前台应用退出的逻辑是通过在Navigator的handleInput()方法判断是否在应用模式的标志位内，然后再通过RTOS的通知机制通知Task杀死自己以退出应用。但是如果是选择裸机开发，建议将按键扫描以及handleInput()方法放入定时器中断扫描按键，以免死循环的应用代码无法通过按键退出，建议写法如下

```c
void Timer_PIT_handler()
{
  uint_8 key_value = key_scanner();
  menu_handle_input(navigator_);
  if (menu_get_app_mode(navigator_))
  {
    if (key_value == LEFT) menu_set_app_mode(navigator_, 0)
  }
}

/// 应用的代码

void myAPP(void **args)
{
  uint8_t data = *(args[0]);  // 参数读取准备工作
  
  // 一些准备工作
  
  while(menu_get_app_mode(navigator_))
  {
    // loop forever
  }
  
}
```

### 2.C++标准

必须使用C++11以上的标准编译器，Keil和TI的编译器是可以设置的

### 3.接口

C++的接口是直接暴露的，可以看其头文件的实现

C版本需要使用`menu_c_wrapper.h`包装器

有以下接口

```c
void* menu_builder(void* mainMenu);   // 构建菜单导航器并返回指针

void menu_delete(void* navigator);    // 结束生命周期删除导航器类

void menu_handle_input(void* navigator, uint8_t key_value);   // 输入处理接口

void menu_refresh_display(void* navigator);   // 显存刷新接口

char* menu_get_display_buffer(void* navigator);   // 读取显存接口

uint8_t menu_get_app_mode(void* navigator);   // 获取当前应用是否运行标志位

void menu_set_app_mode(void* navigator, uint8_t mode);    // 设置应用标志位
```

---

## 👨‍💻 作者

**Taseny**

欢迎反馈问题与建议

> 如果本项目对你有帮助的话还请您帮忙点一下 :start:

---

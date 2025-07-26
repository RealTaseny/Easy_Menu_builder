# C_USAGE_EXAMPLE

## 目录结构

```shell
cpp_usage
├── include
│   └── generated_header.h    // 构建器生成的头文件
├── src
│   └── generated_code.cpp  // 构建器生成的源文件
├── main.c                  // 示例主函数
├── CMakeLists.txt          // CMakeLists
└── README.MD
```

本`Demo`仅演示项目结构, 应为底层没有`OLED`显示接口所以并不能看到实际效果

---

## 接口用法

接口的移植并不困难, 你只需要移植好在`Easy_Menu_Builder/lib/menu_navigator/{include, src}`下的头文件和源文件, 然后如本示例般使用即可

核心只有以下几行代码

菜单导航器接收并处理按键输入

声明一个全局泛型指针
```c
void *navigator; 
```

调用导航菜单包装器初始化这个指针
```c
navigator_ = menu_builder(getMainItem());
```

按键扫描
```c
uint8_t key_value = key_scaner();
menu_handle_input(navigator_, key_value);
```

刷新导航器内部显存(字符串)
```c
menu_refresh_display(navigator_);
```

读取导航器内部显存地址
```c
char *ptrDisplayBuffer = menu_get_display_buffer(navigator_);
```

调用显示接口显示(从[0, 0] 坐标显示字符串即可)
```c
oledDisplay(ptrDisplayBuffer);
```

---

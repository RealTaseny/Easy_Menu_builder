//
// Created by taseny on 25-7-22.
//

#include "code_generator.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

QString CodeGenerator::generateStructCode(const QList<MenuItemEditor::ItemData>& items)
{
    QString code = "";
    code += "struct g_menu_variables_t\n{\n";

    for (const auto& item : items)
    {
        if (item.type == "bool")
        {
            code += QString("    bool %1 = false;\n").arg(item.varName);
        }
    }
    for (const auto& item : items)
    {
        if (item.type == "Changeable")
        {
            code += QString("    %1 %2 = 0;\n").arg(item.dataType).arg(item.varName);
        }
    }

    code += "} g_menu_vars;\n\n";
    return code;
}

QString CodeGenerator::generateMenuCode(const QList<MenuItemEditor::ItemData>& items)
{
    QString code;
    QMap<QString, QList<MenuItemEditor::ItemData>> childrenMap;
    QMap<QString, MenuItemEditor::ItemData> itemMap;
    QStringList arrayCodeList;
    QSet<QString> generatedArrays;

    for (const auto& item : items)
    {
        itemMap[item.name] = item;
        if (!item.parentName.isEmpty())
        {
            childrenMap[item.parentName].append(item);
        }
    }

    for (const auto& item : items)
    {
        if (!item.isRoot) continue;

        QString rootName = item.name;
        QString rootSafeName = rootName.toLower().replace(" ", "_");
        QString mainChildrenArrayName = rootSafeName + "Children";

        generateChildrenArrayRecursive(rootName, childrenMap, itemMap, arrayCodeList, generatedArrays);

        int rootChildCount = childrenMap.contains(rootName) ? childrenMap[rootName].size() : 0;
        code += QString("menuItem* mainMenu = menuItem::createNormalItem(\"%1\", %2, %3);\n")
                .arg(rootName)
                .arg(mainChildrenArrayName)
                .arg(rootChildCount);
    }

    // 拼接所有数组定义（顺序已正确）
    return arrayCodeList.join("") + "\n" + code;
}

void CodeGenerator::generateChildrenArrayRecursive(const QString& parentName,
                                                   const QMap<QString, QList<MenuItemEditor::ItemData>>& childrenMap,
                                                   const QMap<QString, MenuItemEditor::ItemData>& itemMap,
                                                   QStringList& arrayCodeList,
                                                   QSet<QString>& generatedArrays)
{
    if (!childrenMap.contains(parentName)) return;

    QString parentSafeName = parentName.toLower().replace(" ", "_");
    QString arrayName = parentSafeName + "Children";

    // 如果已生成则跳过
    if (generatedArrays.contains(arrayName)) return;
    generatedArrays.insert(arrayName);

    QString arrayCode;
    arrayCode += QString("menuItem* %1[] = {\n").arg(arrayName);

    for (const auto& child : childrenMap[parentName])
    {
        QString childSafeName = child.name.toLower().replace(" ", "_");
        QString childChildrenArray = childSafeName + "Children";
        int childCount = childrenMap.contains(child.name) ? childrenMap[child.name].size() : 0;

        if (child.type == "Normal")
        {
            // 递归生成子项数组
            if (childrenMap.contains(child.name))
            {
                generateChildrenArrayRecursive(child.name, childrenMap, itemMap, arrayCodeList, generatedArrays);
                arrayCode += QString("    menuItem::createNormalItem(\"%1\", %2, %3),\n")
                             .arg(child.name)
                             .arg(childChildrenArray)
                             .arg(childCount);
            }
            else
            {
                arrayCode += QString("    menuItem::createNormalItem(\"%1\", nullptr, 0),\n")
                    .arg(child.name);
            }
        }
        else if (child.type == "Changeable")
        {
            arrayCode += QString("    menuItem::createChangeableItem<%1>(\"%2\", g_menu_vars.%3, %4, %5, %6, ")
                         .arg(child.dataType)
                         .arg(child.name)
                         .arg(child.varName)
                         .arg(child.minValue)
                         .arg(child.maxValue)
                         .arg(child.step);

            if (child.hasCallback && !child.callbackCode.isEmpty())
            {
                arrayCode += QString("[](const %1 value) {\n        /* %2 */\n    }")
                             .arg(child.dataType)
                             .arg(child.callbackCode);
            }
            else
            {
                arrayCode += "nullptr";
            }
            arrayCode += "),\n";
        }
        else if (child.type == "bool")
        {
            arrayCode += QString("    menuItem::createToggle(\"%1\", &g_menu_vars.%2, ")
                         .arg(child.name)
                         .arg(child.varName);

            if (child.hasCallback && !child.callbackCode.isEmpty())
            {
                arrayCode += QString("[](const bool state) {\n        /* %1 */\n    }")
                    .arg(child.callbackCode);
            }
            else
            {
                arrayCode += "nullptr";
            }
            arrayCode += "),\n";
        }
        else if (child.type == "Application")
        {
            QString argsStr = child.argsName.isEmpty() ? "nullptr" : child.argsName;
            QString funcStr = child.funcName.isEmpty() ? "nullptr" : child.funcName;

            arrayCode += QString("    menuItem::createApp(\"%1\", %2, %3),\n")
                         .arg(child.name)
                         .arg(argsStr)
                         .arg(funcStr);
        }
    }

    arrayCode += "};\n\n";
    arrayCodeList.append(arrayCode); // 加入数组代码池
}


QString CodeGenerator::generateCallbackCode(const QList<MenuItemEditor::ItemData>& items)
{
    QString code = "";

    for (const auto& item : items)
    {
        if (item.type == "Application")
        {
            QString funcName;
            if (!item.funcName.isEmpty())
            {
                funcName = item.funcName;
            }
            else
            {
                QString itemNameNoSpaces = item.name;
                itemNameNoSpaces.replace(" ", "");
                funcName = itemNameNoSpaces.toLower() + "App";
            }

            code += QString("void %1(void** args) {\n    /* %2 */\n}\n\n")
                    .arg(funcName)
                    .arg(item.callbackCode);
        }
    }

    for (const auto& item : items)
    {
        if (item.type == "Application")
        {
            QString funcName;
            if (!item.funcName.isEmpty())
            {
                funcName = item.funcName;
            }
            else
            {
                QString itemNameNoSpaces = item.name;
                itemNameNoSpaces.replace(" ", "");
                funcName = itemNameNoSpaces.toLower() + "App";
            }

            QString argName = item.argsName.isEmpty() ? funcName + "Args" : item.argsName;

            if (code.contains(argName))
            {
                continue;
            }

            code += QString("// %1 回调函数参数指针数组\n").arg(item.funcName);
            code += QString("void* %1[] = {\n       nullptr // 请自行添加参数指针\n};\n\n").arg(argName);
        }
    }

    return code;
}

QString CodeGenerator::generateCodePreview(const MenuItemEditor::ItemData& item)
{
    QString code;

    code += "// 单项菜单代码预览\n\n";

    if (item.type == "Changeable")
    {
        code += "// 变量声明\n";
        code += QString("%1 %2 = %3; // 设定初始值\n\n")
                .arg(item.dataType)
                .arg(item.varName.isEmpty() ? QString("g_") + item.name.toLower().replace(" ", "_") : item.varName)
                .arg(item.initialValue);
    }
    else if (item.type == "bool")
    {
        code += "// 变量声明\n";
        code += QString("bool %1 = false; // 初始值为关闭状态\n\n")
            .arg(item.varName.isEmpty() ? QString("g_") + item.name.toLower().replace(" ", "_") : item.varName);
    }

    code += "// 菜单项定义\n";
    if (item.type == "Normal")
    {
        QString varName = item.varName.isEmpty() ? item.name.toLower().replace(" ", "_") + "_menu" : item.varName;
        code += QString("menuItem* %1 = menuItem::createNormalItem(\"%2\", %3Children, %3ChildrenCount);\n")
                .arg(varName)
                .arg(item.name)
                .arg(varName);
    }
    else if (item.type == "Changeable")
    {
        QString varName = item.varName.isEmpty() ? item.name.toLower().replace(" ", "_") : item.varName;
        code += QString("menuItem* %1_item = menuItem::createChangeableItem<%2>(\"%3\", %1, %4, %5, %6, ")
                .arg(varName)
                .arg(item.dataType)
                .arg(item.name)
                .arg(item.minValue)
                .arg(item.maxValue)
                .arg(item.step);

        if (item.hasCallback)
        {
            if (item.callbackCode.isEmpty())
            {
                code += "nullptr";
            }
            else
            {
                code += QString("[](const %1 value) {\n     //Callback Func Zone\n    /* %2 */\n}")
                        .arg(item.dataType)
                        .arg(item.callbackCode);
            }
        }
        else
        {
            code += "nullptr";
        }

        code += ");\n";
    }
    else if (item.type == "bool")
    {
        QString varName = item.varName.isEmpty() ? item.name.toLower().replace(" ", "_") : item.varName;
        code += QString("menuItem* %1_item = menuItem::createToggle(\"%2\", &%1, ")
                .arg(varName)
                .arg(item.name);

        if (item.hasCallback)
        {
            if (item.callbackCode.isEmpty())
            {
                code += "nullptr";
            }
            else
            {
                code += QString("[](const bool state) {\n       //Callback Func Zone\n     /* %1 */\n}")
                    .arg(item.callbackCode);
            }
        }
        else
        {
            code += "nullptr";
        }

        code += ");\n";
    }
    else if (item.type == "Application")
    {
        QString varName = item.varName.isEmpty() ? item.name.toLower().replace(" ", "_") + "_app" : item.varName;
        QString argsName = item.argsName.isEmpty() ? varName + "_args" : item.argsName;

        code += QString("// %1 回调函数参数指针数组\n").arg(item.funcName);
        code += QString("void* %1[] = {\n       // 请自行添加参数指针\n};\n\n").arg(item.argsName);

        code += QString("menuItem* %1_item = menuItem::createApp(\"%2\", (void**)%3, ")
                .arg(varName)
                .arg(item.name)
                .arg(argsName);

        if (item.callbackCode.isEmpty() && item.funcName.isEmpty())
        {
            code += "nullptr";
        }
        else
        {
            QString funcName = item.funcName.isEmpty() ? varName : item.funcName;
            code += funcName;
        }

        code += ");\n";
    }

    code += "\n// 使用示例\n";
    if (item.isRoot)
    {
        code += "Navigator* navigator = new Navigator(mainMenu);\n";
    }
    else if (item.type == "Normal" && !item.isRoot)
    {
        QString varName = item.varName.isEmpty() ? item.name.toLower().replace(" ", "_") + "_menu" : item.varName;
        code += QString("// 添加到父菜单\n");
        code += QString("parentMenu->children_item_[childIndex] = %1;\n")
            .arg(varName);
    }
    else
    {
        QString varName = item.varName.isEmpty() ? item.name.toLower().replace(" ", "_") : item.varName;
        code += QString("// 添加到父菜单\n");
        code += QString("parentMenuChildren[childIndex] = %1_item;\n")
            .arg(varName);
    }

    return code;
}

bool CodeGenerator::saveCodeToFile(const QString& filePath, const QString& code)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "无法打开文件保存代码:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out << code;
    file.close();
    return true;
}

QString CodeGenerator::generateVariableDeclarations(const MenuItemEditor::ItemData& item)
{
    if (item.type == "Changeable")
    {
        return QString("%1 %2 = %3;\n")
               .arg(item.dataType)
               .arg(item.varName)
               .arg(item.minValue); // 默认使用最小值作为初始值
    }
    if (item.type == "bool")
    {
        return QString("bool %1 = false;\n")
            .arg(item.varName);
    }
    return QString();
}

bool CodeGenerator::isFactoryTypeSupportCallback(const QString& type)
{
    return (type == "Changeable" || type == "bool" || type == "Application");
}

QString CodeGenerator::generateAppFunctionParameters(const MenuItemEditor::ItemData& item)
{
    if (item.type == "Application")
    {
        return "void** args - 应用函数参数数组";
    }
    return QString();
}

void CodeGenerator::generateMenuEmulatorCode(const QList<MenuItemEditor::ItemData>& items)
{
    QString code = "";

    // 包含头文件
    code += "#include \"menu_navigator.h\"\n";
    code += "#include <cstdint>\n\n";

    code += "using namespace Menu;\n\n";

    // 生成结构体代码
    code += "// 生成的变量定义\n";
    code += generateStructCode(items);
    code += "\n";

    // 生成回调函数代码
    code += "// 生成的回调函数和参数指针数组\n";
    code += generateCallbackCode(items);
    code += "\n";

    // 生成菜单项代码
    code += "// 生成的菜单项定义\n";
    code += generateMenuCode(items);
    code += "\n";

    code +=
        "\n #ifdef __cplusplus\n \n extern \"C\" {\n #endif\n Navigator* getNavigator() \n { \n return new Navigator(mainMenu); \n }\n #ifdef __cplusplus \n }\n #endif\n\n";

    QString cppFilePath = "/home/taseny/Easy_Menu_Builder/lib/menu_emulator_lib/src/generated_code.cpp";

    QFile cppFile(cppFilePath);
    if (cppFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&cppFile);
        out << code;
        cppFile.close();
    }
    else
    {
        qWarning() << "无法打开文件保存：" << cppFile.errorString();
    }
}

void CodeGenerator::generateMenuEmulatorHeader()
{
    QString code = "";

    code += "#ifndef GENERATED_CODE_H\n";
    code += "#define GENERATED_CODE_H\n\n";

    code += "#include \"menu_navigator.h\"\n\n";

    code += "#ifdef __cplusplus\n";
    code += "extern \"C\" {\n";
    code += "#endif\n\n";

    code += "Menu::Navigator* getNavigator();\n\n";

    code += "#ifdef __cplusplus\n";
    code += "}\n";
    code += "#endif\n\n";
    code += "#endif //GENERATED_CODE_H\n";

    QString headerFilePath = "/home/taseny/Easy_Menu_Builder/lib/menu_emulator_lib/include/generated_code.h";

    QFile headerFile(headerFilePath);
    if (headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&headerFile);
        out << code;
        headerFile.close();
    }
    else
    {
        qWarning() << "无法打开文件保存：" << headerFile.errorString();
    }
}

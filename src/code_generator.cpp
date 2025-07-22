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

    for (const auto& item : items) {
        if (!item.parentName.isEmpty()) {
            childrenMap[item.parentName].append(item);
        }
    }

    for (auto it = childrenMap.constBegin(); it != childrenMap.constEnd(); ++it) {
        // 跳过顶层菜单项（将在第四步单独处理）
        bool isTopLevelParent = false;
        for (const auto& item : items) {
            if (item.name == it.key() && item.isRoot) {
                isTopLevelParent = true;
                break;
            }
        }
        if (isTopLevelParent) continue;

        QString parentSafeName = it.key().toLower().replace(" ", "_");
        code += QString("menuItem* %1Children[] = {\n").arg(parentSafeName);

        for (const auto& item : it.value()) {
            if (item.type == "Normal") {
                code += QString("    menuItem::createNormalItem(\"%1\", nullptr, 0),\n")
                        .arg(item.name);
            }
            else if (item.type == "Changeable") {
                code += QString("    menuItem::createChangeableItem<%1>(\"%2\", g_menu_vars.%3, %4, %5, %6, ")
                        .arg(item.dataType)
                        .arg(item.name)
                        .arg(item.varName)
                        .arg(item.minValue)
                        .arg(item.maxValue)
                        .arg(item.step);

                if (item.hasCallback && !item.callbackCode.isEmpty()) {
                    code += QString("[](const %1 value) {\n        %2\n    }")
                            .arg(item.dataType)
                            .arg(item.callbackCode);
                }
                else {
                    code += "nullptr";
                }
                code += "),\n";
            }
            else if (item.type == "bool") {
                code += QString("    menuItem::createToggle(\"%1\", &g_menu_vars.%2, ")
                        .arg(item.name)
                        .arg(item.varName);

                if (item.hasCallback && !item.callbackCode.isEmpty()) {
                    code += QString("[](const bool state) {\n        %1\n    }")
                            .arg(item.callbackCode);
                }
                else {
                    code += "nullptr";
                }
                code += "),\n";
            }
            else if (item.type == "Application") {
                QString argsStr = item.argsName.isEmpty() ? "nullptr" : item.argsName;
                QString funcStr = item.funcName.isEmpty() ? "nullptr" : item.funcName;

                code += QString("    menuItem::createApp(\"%1\", %2, %3),\n")
                        .arg(item.name)
                        .arg(argsStr)
                        .arg(funcStr);
            }
        }

        code += "};\n\n";
    }

    for (const auto& item : items) {
        if (item.isRoot) {
            QString safeName = item.name.toLower().replace(" ", "_");
            QString mainChildrenArrayName = safeName + "Children";

            code += QString("menuItem* %1[] = {\n").arg(mainChildrenArrayName);

            if (childrenMap.contains(item.name)) {
                for (const auto& child : childrenMap[item.name]) {

                    if ( childrenMap.contains(child.name)) {
                        QString safeChildrenName = child.name.toLower().replace(" ", "_");
                        QString childrenArrayName = safeChildrenName + "Children";
                        int childCount = childrenMap[child.name].size();

                        code += QString("   menuItem::createNormalItem(\"%1\", %2, %3),\n")
                        .arg(child.name)
                        .arg(childrenArrayName)
                        .arg(childCount);
                    } else {

                        code += QString("    menuItem::createNormalItem(\"%1\", nullptr, 0),\n")
                                .arg(child.name);
                    }
                }
            }

            code += "};\n\n";

            int rootChildCount = childrenMap.contains(item.name) ? childrenMap[item.name].size() : 0;
            code += QString("menuItem* mainMenu = menuItem::createNormalItem(\"%1\", %2, %3);\n")
                    .arg(item.name)
                    .arg(mainChildrenArrayName)
                    .arg(rootChildCount);
        }
    }

    return code;
}


QString CodeGenerator::generateCallbackCode(const QList<MenuItemEditor::ItemData>& items)
{
    QString code = "";

    for (const auto& item : items)
    {
        if (item.hasCallback && !item.callbackCode.isEmpty())
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

                code += QString("void %1(void** args) {\n    %2\n}\n\n")
                        .arg(funcName)
                        .arg(item.callbackCode);
            }
        }
    }

    for (const auto& item : items)
    {
        if (item.hasCallback && !item.callbackCode.isEmpty())
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

                code += QString("// %1 回调函数参数指针数组\n").arg(item.funcName);
                code += QString("void* %1[] = {\n       // 请自行添加参数指针\n};\n\n").arg(item.argsName);
            }
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
        QString varName = item.varName.isEmpty() ? 
                          item.name.toLower().replace(" ", "_") + "_menu" : item.varName;
        code += QString("menuItem* %1 = menuItem::createNormalItem(\"%2\", %3Children, %3ChildrenCount);\n")
                .arg(varName)
                .arg(item.name)
                .arg(varName);
    }
    else if (item.type == "Changeable")
    {
        QString varName = item.varName.isEmpty() ? 
                          item.name.toLower().replace(" ", "_") : item.varName;
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
                code += QString("[](const %1 value) {\n     //Callback Func Zone\n   %2\n}")
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
        QString varName = item.varName.isEmpty() ? 
                          item.name.toLower().replace(" ", "_") : item.varName;
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
                code += QString("[](const bool state) {\n       //Callback Func Zone\n    %1\n}")
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
        QString varName = item.varName.isEmpty() ? 
                          item.name.toLower().replace(" ", "_") + "_app" : item.varName;
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
        QString varName = item.varName.isEmpty() ? 
                          item.name.toLower().replace(" ", "_") + "_menu" : item.varName;
        code += QString("// 添加到父菜单\n");
        code += QString("parentMenu->children_item_[childIndex] = %1;\n")
                .arg(varName);
    }
    else
    {
        QString varName = item.varName.isEmpty() ? 
                          item.name.toLower().replace(" ", "_") : item.varName;
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

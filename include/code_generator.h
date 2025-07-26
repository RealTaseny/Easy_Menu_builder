//
// Created by taseny on 25-7-21.
//

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "menu_item_editor.h"

class CodeGenerator
{
public:
    // 生成结构体代码
    static QString generateStructCode(const QList<MenuItemEditor::ItemData>& items);
    // 生成菜单项定义代码
    static QString generateMenuCode(const QList<MenuItemEditor::ItemData>& items);
    // 生成回调函数代码
    static QString generateCallbackCode(const QList<MenuItemEditor::ItemData>& items);
    // 从一个节点生成完整代码预览
    static QString generateCodePreview(const MenuItemEditor::ItemData& item);
    // 保存生成的代码到文件
    static bool saveCodeToFile(const QString& filePath, const QString& code);

    static void generateMenuEmulatorCode(const QList<MenuItemEditor::ItemData>& items);

    static void generateMenuEmulatorHeader();

    static void generateChildrenArrayRecursive(const QString& parentName,
                                    const QMap<QString, QList<MenuItemEditor::ItemData>>& childrenMap,
                                    const QMap<QString, MenuItemEditor::ItemData>& itemMap,
                                    QStringList& arrayCodeList,
                                    QSet<QString>& generatedArrays);

private:
    // 生成变量声明
    static QString generateVariableDeclarations(const MenuItemEditor::ItemData& item);
    // 检查工厂类型是否支持回调
    static bool isFactoryTypeSupportCallback(const QString& type);
    // 生成应用函数参数预览
    static QString generateAppFunctionParameters(const MenuItemEditor::ItemData& item);
};


#endif //CODE_GENERATOR_H

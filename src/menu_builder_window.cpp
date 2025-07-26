//
// Created by taseny on 25-7-21.
//

#include "menu_builder_window.h"
#include "code_generator.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QInputDialog>
#include <QTextEdit>
#include <QDockWidget>
#include <QMenu>
#include <QToolButton>
#include <QToolBar>
#include <QTreeWidget>
#include <QJsonArray>

MenuBuilderWindow::MenuBuilderWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
}

void MenuBuilderWindow::onItemDataModified(const MenuItemEditor::ItemData& data)
{
    if (!currentItem)
    {
        return;
    }

    // 获取项目当前数据，用于检测是否有实际变化
    QString oldName = currentItem->text(0);
    QString oldType = currentItem->text(1);

    // 保持父项名称
    MenuItemEditor::ItemData newData = data;
    if (currentItem->parent())
    {
        QVariant parentVarData = currentItem->parent()->data(0, Qt::UserRole);
        if (parentVarData.isValid())
        {
            MenuItemEditor::ItemData parentData = parentVarData.value<MenuItemEditor::ItemData>();
            newData.parentName = parentData.name;
        }
    }

    // 应用修改的数据
    applyItemData(newData);

    // 如果有实际变化，保存状态
    if (oldName != currentItem->text(0) || oldType != currentItem->text(1))
    {
        saveState();
    }

    // 刷新树视图，确保显示最新数据
    menuTree->update();

    // 更新代码预览
    QString code;
    updateCodePreview(code);
    codePreview->setText(code);
}

void MenuBuilderWindow::onMenuItemSelectionChanged(const QModelIndex& current)
{
    if (!current.isValid())
    {
        return;
    }

    // 获取当前选中项
    QTreeWidgetItem* item = menuTree->currentItem();
    if (!item) return;

    currentItem = item;

    // 获取当前选中项的数据
    MenuItemEditor::ItemData data;
    data.name = item->text(0);
    data.type = item->text(1);

    // 设置是否为根节点
    data.isRoot = !item->parent();

    // 更新编辑器
    itemEditor->setItemData(data);
}

void MenuBuilderWindow::onNewMenu()
{
    // 清空撤销和重做栈
    undoStack.clear();
    redoStack.clear();

    menuTree->clear();
    auto* rootItem = new QTreeWidgetItem(menuTree);

    QString projectName = QInputDialog::getText(this, tr("新建工程"), tr("请输入工程名称:"));
    if (projectName.isEmpty())
    {
        QMessageBox::warning(this, tr("[WARNING]"), tr("工程名称不能为空!"));
        return;
    }
    m_project_name_ = projectName;

    rootItem->setText(0, "Main Menu");
    rootItem->setText(1, "Normal");
    menuTree->setCurrentItem(rootItem);

    // 保存初始状态
    saveState();
}

void MenuBuilderWindow::onAddItem()
{
    if (!hasActiveProject())
    {
        QMessageBox::warning(this, "[WARNING]", "请新建工程!");
        return;
    }
    auto* current = menuTree->currentItem();
    if (!current) return;

    // 保存当前状态用于撤销
    saveState();

    auto* newItem = new QTreeWidgetItem(current);
    newItem->setText(0, "New Item");
    newItem->setText(1, "Normal");

    // 创建并设置新项目的数据
    MenuItemEditor::ItemData data;
    data.name = "New Item";
    data.type = "Normal";
    data.isRoot = false;

    // 从父项获取变量名作为parentName
    QVariant parentVarData = current->data(0, Qt::UserRole);
    if (parentVarData.isValid())
    {
        MenuItemEditor::ItemData parentData = parentVarData.value<MenuItemEditor::ItemData>();
        data.parentName = parentData.name; // 设置父项的变量名
    }

    // 保存数据到项目中
    QVariant varData;
    varData.setValue(data);
    newItem->setData(0, Qt::UserRole, varData);

    current->setExpanded(true);
    menuTree->setCurrentItem(newItem);
}

void MenuBuilderWindow::onRemoveItem()
{
    if (!hasActiveProject())
    {
        QMessageBox::warning(this, "[WARNING]", "请先新建工程!");
        return;
    }
    auto* current = menuTree->currentItem();
    if (!current) return; // Don't remove root item

    if (!current->parent())
    {
        QMessageBox::warning(this, "[ERROR]", "不能删除主菜单!");
        return;
    }

    // 保存当前状态用于撤销
    saveState();

    delete current;
}

bool MenuBuilderWindow::hasActiveProject()
{
    // 检查是否有活动的项目
    return m_project_name_.isEmpty() == false;
}


void MenuBuilderWindow::onItemSelected(QTreeWidgetItem* current)
{
    if (!current)
    {
        propertyDock->setEnabled(false);
        return;
    }

    // 保存当前选中的项目
    currentItem = current;

    propertyDock->setEnabled(true);

    // 尝试从项目的自定义数据中获取完整的项目数据
    QVariant varData = current->data(0, Qt::UserRole);
    if (varData.isValid())
    {
        // 如果存在自定义数据，直接使用
        MenuItemEditor::ItemData data = varData.value<MenuItemEditor::ItemData>();
        itemEditor->setItemData(data);
    }
    else
    {
        // 否则，创建基本数据对象
        MenuItemEditor::ItemData data;
        data.name = current->text(0);
        data.type = current->text(1);
        data.isRoot = !current->parent();
        itemEditor->setItemData(data);
    }
}

void MenuBuilderWindow::onGenerateCode()
{
    QString code;

    // 检查项目是否存在
    if (!hasActiveProject())
    {
        QMessageBox::warning(this, tr("[WARNING]"), tr("请先新建或加载一个项目!"));
        return;
    }

    // 检查菜单树是否有内容
    if (menuTree->topLevelItemCount() == 0)
    {
        code = "// 菜单树为空，请先创建菜单项";
        codePreview->setText(code);
        return;
    }

    // 收集所有菜单项数据
    QList<MenuItemEditor::ItemData> items;
    QJsonArray menuArray;
    saveMenuTree(menuTree->topLevelItem(0), menuArray);
    parseMenuTree(menuArray, items);

    // 包含头文件
    code += "#include \"menu.h\"\n";
    code += "#include <cstdint>\n\n";

    code += "using namespace Menu;\n\n";

    // 生成结构体代码
    code += "// 生成的变量定义\n";
    code += CodeGenerator::generateStructCode(items);
    code += "\n";

    // 生成回调函数代码
    code += "// 生成的回调函数和参数指针数组\n";
    code += CodeGenerator::generateCallbackCode(items);
    code += "\n";

    // 生成菜单项代码
    code += "// 生成的菜单项定义\n";
    code += CodeGenerator::generateMenuCode(items);
    code += "\n";

    // 添加导航器初始化代码
    code += "// 菜单导航器初始化\n";
    code += "Navigator* navigator = new Menu::Navigator(mainMenu);\n";

    codePreview->setText(code);

    CodeGenerator::generateMenuEmulatorHeader();

    CodeGenerator::generateMenuEmulatorCode(items);

    emit menuStructureUpdated(items);
}

void MenuBuilderWindow::updateCodePreview(QString& code)
{
    // 检查是否有有效的选中项
    if (!currentItem)
    {
        code = "// 请选择一个菜单项来查看代码预览";
        return;
    }

    // 获取当前选中项的数据
    QVariant varData = currentItem->data(0, Qt::UserRole);
    if (varData.isValid())
    {
        MenuItemEditor::ItemData itemData = varData.value<MenuItemEditor::ItemData>();

        // 如果当前编辑器有最新的数据，则使用编辑器的数据
        if (currentItem == menuTree->currentItem())
        {
            itemData = itemEditor->getItemData();
        }

        code = CodeGenerator::generateCodePreview(itemData);

        // 添加额外说明
        if (itemData.isRoot)
        {
            code += "\n// 注意：这是主菜单项，将作为菜单系统的入口点。\n";
        }

        // 如果是子菜单，添加提示信息
        if (itemData.type == "Normal" && currentItem->childCount() > 0)
        {
            code += "\n// 注意：此菜单项包含" + QString::number(currentItem->childCount()) + "个子项。\n";
            code += "// 在实际代码中，需要创建子项数组并传递给createNormalItem函数。\n";
        }
    }
    else
    {
        code = "// 当前选中项没有有效数据";
    }
}

void MenuBuilderWindow::saveMenuTree(QTreeWidgetItem* item, QJsonArray& array)
{
    if (!item) return;

    QJsonObject itemObj;

    // 保存基本属性
    itemObj["name"] = item->text(0);
    itemObj["type"] = item->text(1);

    // 获取完整的项目数据
    QVariant varData = item->data(0, Qt::UserRole);
    if (varData.isValid())
    {
        MenuItemEditor::ItemData data = varData.value<MenuItemEditor::ItemData>();
        saveItemData(data, itemObj);
    }

    // 获取完整的项目数据
    if (item == currentItem)
    {
        // 如果是当前选中的项目，直接从编辑器获取最新数据
        MenuItemEditor::ItemData data = itemEditor->getItemData();
        saveItemData(data, itemObj);
    }
    else
    {
        // 否则，从项目的自定义数据中获取
        QVariant varData = item->data(0, Qt::UserRole);
        if (varData.isValid())
        {
            MenuItemEditor::ItemData data = varData.value<MenuItemEditor::ItemData>();
            saveItemData(data, itemObj);
        }
    }

    // 保存子项目
    QJsonArray children;
    for (int i = 0; i < item->childCount(); i++)
    {
        saveMenuTree(item->child(i), children);
    }
    if (!children.isEmpty())
    {
        itemObj["children"] = children;
    }

    array.append(itemObj);
}

void MenuBuilderWindow::loadMenuTree(QTreeWidgetItem* parent, const QJsonObject& itemObj)
{
    QTreeWidgetItem* item;
    if (parent)
    {
        item = new QTreeWidgetItem(parent);
    }
    else
    {
        item = new QTreeWidgetItem(menuTree);
    }

    // 设置基本属性
    // 设置基本属性
    item->setText(0, itemObj["name"].toString());
    item->setText(1, itemObj["type"].toString());

    // 创建完整的项目数据并存储
    MenuItemEditor::ItemData data;
    data.name = itemObj["name"].toString();
    data.type = itemObj["type"].toString();

    // 加载其他属性（如果存在）
    if (itemObj.contains("parentName")) data.parentName = itemObj["parentName"].toString();
    if (itemObj.contains("varName")) data.varName = itemObj["varName"].toString();
    if (itemObj.contains("dataType")) data.dataType = itemObj["dataType"].toString();
    if (itemObj.contains("initialValue")) data.initialValue = itemObj["initialValue"].toDouble();
    if (itemObj.contains("minValue")) data.minValue = itemObj["minValue"].toDouble();
    if (itemObj.contains("maxValue")) data.maxValue = itemObj["maxValue"].toDouble();
    if (itemObj.contains("step")) data.step = itemObj["step"].toDouble();
    if (itemObj.contains("hasCallback")) data.hasCallback = itemObj["hasCallback"].toBool();
    if (itemObj.contains("callbackCode")) data.callbackCode = itemObj["callbackCode"].toString();
    if (itemObj.contains("funcName")) data.funcName = itemObj["funcName"].toString();
    if (itemObj.contains("argsName")) data.argsName = itemObj["argsName"].toString();
    if (itemObj.contains("isRoot")) data.isRoot = itemObj["isRoot"].toBool();
    else data.isRoot = !parent;

    // 将数据存储在项目的自定义数据中
    QVariant varData;
    varData.setValue(data);
    item->setData(0, Qt::UserRole, varData);

    // 加载子项目
    if (itemObj.contains("children"))
    {
        QJsonArray children = itemObj["children"].toArray();
        for (const auto& child : children)
        {
            loadMenuTree(item, child.toObject());
        }
    }

    item->setExpanded(true);
}

void MenuBuilderWindow::saveState()
{
    // 创建保存菜单树结构的JSON数组
    QJsonArray menuArray;
    if (menuTree->topLevelItemCount() > 0)
    {
        saveMenuTree(menuTree->topLevelItem(0), menuArray);
    }

    MenuState state;
    state.projectName = m_project_name_;

    // 将JSON数组转换为字符串并保存
    QJsonDocument doc(menuArray);
    state.treeState = doc.toJson();

    undoStack.push(state);
    redoStack.clear();
}

void MenuBuilderWindow::undo()
{
    if (undoStack.isEmpty()) return;

    // 保存当前状态到重做栈
    QJsonArray currentMenuArray;
    if (menuTree->topLevelItemCount() > 0)
    {
        saveMenuTree(menuTree->topLevelItem(0), currentMenuArray);
    }

    MenuState currentState;
    currentState.projectName = m_project_name_;
    QJsonDocument currentDoc(currentMenuArray);
    currentState.treeState = currentDoc.toJson();
    redoStack.push(currentState);

    // 恢复之前的状态
    MenuState prevState = undoStack.pop();
    m_project_name_ = prevState.projectName;

    // 解析之前的菜单树状态
    QJsonDocument doc = QJsonDocument::fromJson(prevState.treeState);
    QJsonArray menuArray = doc.array();

    // 清空当前树并重建
    menuTree->clear();
    if (!menuArray.isEmpty())
    {
        loadMenuTree(nullptr, menuArray[0].toObject());
    }

    // 刷新UI
    menuTree->update();
}

void MenuBuilderWindow::redo()
{
    if (redoStack.isEmpty()) return;

    QJsonArray currentMenuArray;
    if (menuTree->topLevelItemCount() > 0)
    {
        saveMenuTree(menuTree->topLevelItem(0), currentMenuArray);
    }

    MenuState currentState;
    currentState.projectName = m_project_name_;
    QJsonDocument currentDoc(currentMenuArray);
    currentState.treeState = currentDoc.toJson();
    undoStack.push(currentState);

    MenuState nextState = redoStack.pop();
    m_project_name_ = nextState.projectName;

    QJsonDocument doc = QJsonDocument::fromJson(nextState.treeState);
    QJsonArray menuArray = doc.array();

    menuTree->clear();
    if (!menuArray.isEmpty())
    {
        loadMenuTree(nullptr, menuArray[0].toObject());
    }

    // 刷新UI
    menuTree->update();
}


void MenuBuilderWindow::onSaveProject()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("保存配置"), m_project_name_ + ".mProj",
                                                    tr("菜单项目文件 (*.mProj);;所有文件 (*)"), nullptr,
                                                    QFileDialog::DontConfirmOverwrite);

    if (!fileName.isEmpty() && !fileName.endsWith(".mProj", Qt::CaseInsensitive))
    {
        fileName += ".mProj";
    }

    if (!fileName.isEmpty() && QFile::exists(fileName))
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  tr("确认覆盖"),
                                                                  tr("文件 %1 已存在，是否覆盖?").arg(fileName),
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes)
        {
            return;
        }
    }

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("错误"),
                             tr("无法写入文件 %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QJsonObject config;

    config["projectName"] = m_project_name_;

    QJsonArray menuArray;
    saveMenuTree(menuTree->topLevelItem(0), menuArray);
    config["menuTree"] = menuArray;

    QJsonDocument doc(config);
    file.write(doc.toJson());
}


void MenuBuilderWindow::onLoadProject()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("加载菜单项目"), "",
                                                    tr("菜单项目文件 (*.mProj);;所有文件 (*)"), nullptr, QFileDialog::Options());

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("错误"),
                             tr("无法读取文件 %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject config = doc.object();

    m_project_name_ = config["projectName"].toString();

    menuTree->clear();
    QJsonArray menuArray = config["menuTree"].toArray();
    if (!menuArray.isEmpty())
    {
        loadMenuTree(nullptr, menuArray[0].toObject());
    }
}

void MenuBuilderWindow::setupUI()
{
    setWindowTitle(tr("菜单构建器"));
    resize(1600, 900);

    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    setupDockWidgets();
    setupConnections();
}

void MenuBuilderWindow::setupMenuBar()
{
    auto* fileMenu = menuBar()->addMenu(tr("文件"));
    fileMenu->addAction(tr("新建"), this, &MenuBuilderWindow::onNewMenu);
    fileMenu->addAction(tr("保存"), this, &MenuBuilderWindow::onSaveProject);
    fileMenu->addAction(tr("加载"), this, &MenuBuilderWindow::onLoadProject);

    auto* editMenu = menuBar()->addMenu(tr("编辑"));
    editMenu->addAction(tr("撤销"), this, &MenuBuilderWindow::undo, QKeySequence::Undo);
    editMenu->addAction(tr("重做"), this, &MenuBuilderWindow::redo, QKeySequence::Redo);
}

void MenuBuilderWindow::setupToolBar()
{
    mainToolBar = addToolBar(tr("工具栏"));
    mainToolBar->setMovable(false);

    mainToolBar->addAction(tr("添加项"), this, &MenuBuilderWindow::onAddItem);
    mainToolBar->addAction(tr("删除项"), this, &MenuBuilderWindow::onRemoveItem);

    codeGenMenu = new QMenu(tr("生成代码"), this);
    codeGenMenu->addAction(tr("生成C++代码 & 实时预览"), this, &MenuBuilderWindow::onGenerateCode);
    // codeGenMenu->addAction(tr("生成C代码"), this, &MenuBuilderWindow::onGenerateCode);

    auto* codeGenButton = new QToolButton;
    codeGenButton->setMenu(codeGenMenu);
    codeGenButton->setPopupMode(QToolButton::InstantPopup);
    codeGenButton->setText(tr("生成代码"));
    mainToolBar->addWidget(codeGenButton);
}

void MenuBuilderWindow::setupCentralWidget()
{
    auto* centralWidget = new QWidget(this);
    auto* layout = new QHBoxLayout(centralWidget);

    menuTree = new QTreeWidget(this);
    menuTree->setHeaderLabels({tr("菜单项"), tr("类型")});
    menuTree->setDragEnabled(true);
    menuTree->setAcceptDrops(true);
    menuTree->setDropIndicatorShown(true);
    menuTree->setDragDropMode(QAbstractItemView::InternalMove);

    layout->addWidget(menuTree);
    setCentralWidget(centralWidget);

    connect(menuTree, &QTreeWidget::currentItemChanged,
            this, &MenuBuilderWindow::onItemSelected);
}

void MenuBuilderWindow::setupDockWidgets()
{
    propertyDock = new QDockWidget(tr("属性编辑器"), this);
    itemEditor = new MenuItemEditor(propertyDock);
    propertyDock->setWidget(itemEditor);
    addDockWidget(Qt::RightDockWidgetArea, propertyDock);

    // 代码预览
    auto* previewDock = new QDockWidget(tr("代码预览"), this);
    codePreview = new QTextEdit(previewDock);
    codePreview->setReadOnly(true);
    previewDock->setWidget(codePreview);
    addDockWidget(Qt::BottomDockWidgetArea, previewDock);
}

void MenuBuilderWindow::applyItemData(const MenuItemEditor::ItemData& data)
{
    auto* item = menuTree->currentItem();
    if (!item)
    {
        return;
    }

    if (!item->parent())
    {
        if (item->text(0) != "Main Menu")
        {
            item->setText(0, "Main Menu");
        }
    }
    else
    {
        item->setText(0, data.name);
        item->setText(1, data.type);
    }

    QVariant varData;
    varData.setValue(data);
    item->setData(0, Qt::UserRole, varData);
}


bool MenuBuilderWindow::isRootItem(QTreeWidgetItem* item) const
{
    return item && !item->parent();
}

void MenuBuilderWindow::saveItemData(const MenuItemEditor::ItemData& data, QJsonObject& itemObj)
{
    itemObj["parentName"] = data.parentName;
    itemObj["varName"] = data.varName;
    itemObj["dataType"] = data.dataType;
    itemObj["initialValue"] = data.initialValue;
    itemObj["minValue"] = data.minValue;
    itemObj["maxValue"] = data.maxValue;
    itemObj["step"] = data.step;
    itemObj["hasCallback"] = data.hasCallback;
    itemObj["callbackCode"] = data.callbackCode;
    itemObj["funcName"] = data.funcName;
    itemObj["argsName"] = data.argsName;
    itemObj["isRoot"] = data.isRoot;
}

void MenuBuilderWindow::setupConnections()
{
    // 连接树视图选择变化信号
    connect(menuTree, &QTreeWidget::currentItemChanged,
            this, &MenuBuilderWindow::onItemSelected);

    // 连接编辑器数据修改信号
    connect(itemEditor, &MenuItemEditor::itemModified,
            this, &MenuBuilderWindow::onItemDataModified);
}

void MenuBuilderWindow::parseMenuTree(const QJsonArray& menuArray, QList<MenuItemEditor::ItemData>& items)
{
    for (int i = 0; i < menuArray.size(); i++)
    {
        QJsonObject itemObj = menuArray[i].toObject();

        // 创建项目数据
        MenuItemEditor::ItemData data;
        data.name = itemObj["name"].toString();
        data.type = itemObj["type"].toString();
        data.isRoot = itemObj["isRoot"].toBool();

        // 提取其他属性
        if (itemObj.contains("parentName")) data.parentName = itemObj["parentName"].toString();
        if (itemObj.contains("varName")) data.varName = itemObj["varName"].toString();
        if (itemObj.contains("dataType")) data.dataType = itemObj["dataType"].toString();
        if (itemObj.contains("minValue")) data.minValue = itemObj["minValue"].toDouble();
        if (itemObj.contains("maxValue")) data.maxValue = itemObj["maxValue"].toDouble();
        if (itemObj.contains("step")) data.step = itemObj["step"].toDouble();
        if (itemObj.contains("initialValue")) data.initialValue = itemObj["initialValue"].toDouble();
        if (itemObj.contains("hasCallback")) data.hasCallback = itemObj["hasCallback"].toBool();
        if (itemObj.contains("callbackCode")) data.callbackCode = itemObj["callbackCode"].toString();
        if (itemObj.contains("funcName")) data.funcName = itemObj["funcName"].toString();
        if (itemObj.contains("argsName")) data.argsName = itemObj["argsName"].toString();

        // 如果是可变项但数据类型为空，设置默认数据类型
        if (data.type == "Changeable" && data.dataType.isEmpty())
        {
            data.dataType = "int";
        }

        // 如果是应用项但函数名为空，设置默认函数名
        if (data.type == "App" && data.funcName.isEmpty())
        {
            data.funcName = "on" + data.name.replace(" ", "");
        }

        // 添加到列表
        items.append(data);

        // 递归处理子项目，使用当前项的变量名作为父路径
        if (itemObj.contains("children"))
        {
            QJsonArray children = itemObj["children"].toArray();
            parseMenuTree(children, items);
        }
    }
}

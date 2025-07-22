//
// Created by taseny on 25-7-21.
//

#ifndef MENUBUILDER_WINDOW_H
#define MENUBUILDER_WINDOW_H

#pragma once

#include <QMainWindow>
#include <QTreeWidget>
#include <QDockWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QStack>
#include <QStandardItemModel>
#include "menu_item_editor.h"

class MenuBuilderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuBuilderWindow(QWidget* parent = nullptr);
    ~MenuBuilderWindow() = default;

private slots:
    void onItemDataModified(const MenuItemEditor::ItemData& data);
    void onMenuItemSelectionChanged(const QModelIndex& current);
    void onNewMenu();
    void onAddItem();
    void onRemoveItem();
    void onItemSelected(QTreeWidgetItem* current);
    void onGenerateCode();
    void onSaveProject();
    void onLoadProject();

private:
    void saveItemData(const MenuItemEditor::ItemData& data, QJsonObject& itemObj);
    void applyItemData(const MenuItemEditor::ItemData& data);
    bool isRootItem(QTreeWidgetItem* item) const;
    void saveMenuTree(QTreeWidgetItem* item, QJsonArray& array);
    void loadMenuTree(QTreeWidgetItem* parent, const QJsonObject& itemObj);
    void parseMenuTree(const QJsonArray& menuArray, QList<MenuItemEditor::ItemData>& items);
    void saveState();
    void undo();
    void redo();
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupDockWidgets();
    void setupConnections();

    void updateCodePreview(QString& code);
    bool hasActiveProject();

    QString m_project_name_;

    struct MenuState
    {
        QString projectName;
        QByteArray treeState; // 现在存储JSON格式的树结构，而不只是几何信息
    };

    QTreeWidgetItem* currentItem = nullptr; // 替代 currentIndex
    QToolBar* mainToolBar;
    QTreeWidget* menuTree;
    QTextEdit* codePreview;
    QDockWidget* propertyDock;
    MenuItemEditor* itemEditor;
    QMenu* codeGenMenu;

    QStack<MenuState> undoStack;
    QStack<MenuState> redoStack;
};

#endif //MENUBUILDER_WINDOW_H

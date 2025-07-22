//
// Created by taseny on 25-7-21.
//

#ifndef MENU_ITEM_EDITOR_H
#define MENU_ITEM_EDITOR_H

#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTimer>
#include <QStyle>

class MenuItemEditor : public QWidget
{
    Q_OBJECT

public:
    struct ItemData
    {
        QString name;
        QString type;
        QString dataType;
        QString parentName;
        QString varName;
        QString funcName; // 应用类型的函数名
        QString argsName; // 应用类型的参数名
        double initialValue;
        double minValue;
        double maxValue;
        double step;
        bool hasCallback;
        QString callbackCode;
        bool isRoot = false;
    };

    explicit MenuItemEditor(QWidget* parent = nullptr);
    void setItemData(const ItemData& data);
    ItemData getItemData() const;

signals:
    void dataChanged(); // 当编辑器中的数据发生变化时发出
    void itemModified(const ItemData& data); // 添加新的信号用于实时同步数据

private slots:
    void onTypeChanged(int index);
    void onDataChanged();
    void onDataTypeChanged(int index); // 数据类型变化槽函数
    void emitModifiedData(); // 添加新的槽函数
    void onGenerateCodePreview(); // 生成代码预览
    void onSaveCodeToFile(); // 保存代码到文件
    void onCallbackToggled(bool checked); // 回调复选框切换

private:
    void setupUI();
    void setupConnections();
    void updateVisibility();
    void updateEditability();

    QLineEdit* nameEdit;
    QLineEdit* varNameEdit;
    QLineEdit* funcNameEdit; // 应用类型的函数名输入框
    QLineEdit* argsNameEdit; // 应用类型的参数名输入框
    QWidget* appWidget; // 应用类型专用控件容器
    QLabel* varNameLabel; // 变量名标签
    QComboBox* typeCombo;
    QComboBox* dataTypeCombo;
    QDoubleSpinBox* initialValueEdit;
    QDoubleSpinBox* minValueEdit;
    QDoubleSpinBox* maxValueEdit;
    QDoubleSpinBox* stepEdit;
    QCheckBox* callbackCheck;
    QTextEdit* callbackEdit;
    QWidget* changeableWidget;
    QLabel* functionParamsLabel; // 函数参数提示标签
    QPushButton* codePreviewButton; // 代码预览按钮
    QTextEdit* codePreviewEdit; // 代码预览文本框
    QPushButton* saveCodeButton; // 保存代码按钮

    ItemData currentData; // 存储当前数据
    bool isRootNode = false;
};

// 声明为Qt元类型，使其可以在QVariant中使用
Q_DECLARE_METATYPE(MenuItemEditor::ItemData)

#endif //MENU_ITEM_EDITOR_H

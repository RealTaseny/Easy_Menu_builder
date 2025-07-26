//
// Created by taseny on 25-7-21.
//

#ifndef MENU_ITEM_EDITOR_H
#define MENU_ITEM_EDITOR_H

#pragma once

#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTimer>

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
    void dataChanged();
    void itemModified(const ItemData& data);

private slots:
    void onTypeChanged(int index);
    void onDataChanged();
    void onDataTypeChanged(int index);
    void emitModifiedData();
    void onGenerateCodePreview();
    void onCallbackToggled(bool checked);

private:
    void setupUI();
    void setupConnections();
    void updateVisibility();
    void updateEditability();

    QLineEdit* nameEdit;
    QLineEdit* varNameEdit;
    QLineEdit* funcNameEdit;
    QLineEdit* argsNameEdit;
    QWidget* appWidget;
    QLabel* varNameLabel;
    QComboBox* typeCombo;
    QComboBox* dataTypeCombo;
    QDoubleSpinBox* initialValueEdit;
    QDoubleSpinBox* minValueEdit;
    QDoubleSpinBox* maxValueEdit;
    QDoubleSpinBox* stepEdit;
    QCheckBox* callbackCheck;
    QTextEdit* callbackEdit;
    QWidget* changeableWidget;
    QLabel* functionParamsLabel;
    QPushButton* codePreviewButton;
    QTextEdit* codePreviewEdit;

    ItemData currentData; // 存储当前数据
    bool isRootNode = false;
};

// 声明为Qt元类型，使其可以在QVariant中使用
Q_DECLARE_METATYPE(MenuItemEditor::ItemData)

#endif //MENU_ITEM_EDITOR_H

//
// Created by taseny on 25-7-21.
//

#include "menu_item_editor.h"
#include "code_generator.h"

#include <QFileDialog>
#include <QMessageBox>

MenuItemEditor::MenuItemEditor(QWidget* parent)
    : QWidget(parent)
{
    qRegisterMetaType<ItemData>("MenuItemEditor::ItemData");

    setupUI();
    setupConnections();

    QString currentType = typeCombo->currentText();
    if (currentType == tr("Normal"))
    {
        callbackCheck->setEnabled(false);
        callbackCheck->setChecked(false);
        callbackEdit->setVisible(false);
        functionParamsLabel->setVisible(false);

        callbackCheck->setStyleSheet("QCheckBox { color: gray; }");
        callbackCheck->style()->unpolish(callbackCheck);
        callbackCheck->style()->polish(callbackCheck);
        callbackCheck->update();
    }

    onTypeChanged(typeCombo->currentIndex());
}

void MenuItemEditor::onTypeChanged(int index)
{
    QString currentType = typeCombo->currentText();

    bool needsVarName = (currentType == tr("Changeable") || currentType == tr("bool"));
    varNameEdit->setVisible(needsVarName);
    varNameLabel->setVisible(needsVarName);

    bool isAppType = (currentType == tr("Application"));
    appWidget->setVisible(isAppType);

    if (currentType == tr("Normal"))
    {
        callbackCheck->setChecked(false);
        callbackCheck->setEnabled(false);
        callbackEdit->setVisible(false);
        functionParamsLabel->setVisible(false);

        callbackCheck->setStyleSheet("QCheckBox { color: gray; }");
        callbackCheck->style()->unpolish(callbackCheck);
        callbackCheck->style()->polish(callbackCheck);
        callbackCheck->update(); // 强制重绘
    }
    else
    {
        callbackCheck->setStyleSheet("");
    }

    bool supportsCallback = (currentType == tr("Changeable") || currentType == tr("bool"));
    bool isNormalType = (currentType == tr("Normal"));

    callbackCheck->setEnabled(supportsCallback);

    if (isNormalType)
    {
        callbackCheck->setChecked(false);
    }

    if (currentType == tr("Application"))
    {
        callbackCheck->setChecked(true);
        callbackCheck->setEnabled(false);
        callbackEdit->setVisible(true);
        functionParamsLabel->setText(tr("void** args - 应用函数参数数组"));
        functionParamsLabel->setVisible(true);
        callbackEdit->setPlaceholderText(tr("输入应用函数代码，可以通过args参数访问传入的参数"));
    }
    else
    {
        if (!supportsCallback)
        {
            callbackCheck->setChecked(false);
            callbackEdit->setVisible(false);
            functionParamsLabel->setVisible(false);
        }
        else
        {
            bool hasCallback = callbackCheck->isChecked();
            callbackEdit->setVisible(hasCallback);

            if (hasCallback)
            {
                if (currentType == tr("Changeable"))
                {
                    QString dataType = dataTypeCombo->currentText();
                    functionParamsLabel->setText(tr("const %1 value - 当前值").arg(dataType));
                    callbackEdit->setPlaceholderText(tr("输入回调函数代码，可以通过value参数访问当前值"));
                }
                else if (currentType == tr("bool"))
                {
                    functionParamsLabel->setText(tr("const bool state - 当前状态"));
                    callbackEdit->setPlaceholderText(tr("输入回调函数代码，可以通过state参数访问当前状态"));
                }
                functionParamsLabel->setVisible(true);
            }
            else
            {
                functionParamsLabel->setVisible(false);
            }
        }
    }

    updateVisibility();
    onDataChanged();
}

void MenuItemEditor::onDataTypeChanged(int index)
{
    QString dataType = dataTypeCombo->currentText();

    if (dataType == "uint8_t")
    {
        minValueEdit->setRange(0, 255);
        maxValueEdit->setRange(0, 255);
        initialValueEdit->setRange(0, 255);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "uint16_t")
    {
        minValueEdit->setRange(0, 65535);
        maxValueEdit->setRange(0, 65535);
        initialValueEdit->setRange(0, 65535);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "uint32_t")
    {
        minValueEdit->setRange(0, 4294967295.0);
        maxValueEdit->setRange(0, 4294967295.0);
        initialValueEdit->setRange(0, 4294967295.0);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "uint64_t")
    {
        minValueEdit->setRange(0, 9223372036854775807.0); // 近似值
        maxValueEdit->setRange(0, 9223372036854775807.0);
        initialValueEdit->setRange(0, 9223372036854775807.0);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "int8_t")
    {
        minValueEdit->setRange(-128, 127);
        maxValueEdit->setRange(-128, 127);
        initialValueEdit->setRange(-128, 127);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "int16_t")
    {
        minValueEdit->setRange(-32768, 32767);
        maxValueEdit->setRange(-32768, 32767);
        initialValueEdit->setRange(-32768, 32767);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "int32_t")
    {
        minValueEdit->setRange(-2147483648.0, 2147483647.0);
        maxValueEdit->setRange(-2147483648.0, 2147483647.0);
        initialValueEdit->setRange(-2147483648.0, 2147483647.0);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "int64_t")
    {
        minValueEdit->setRange(-9223372036854775807.0, 9223372036854775807.0); // 近似值
        maxValueEdit->setRange(-9223372036854775807.0, 9223372036854775807.0);
        initialValueEdit->setRange(-9223372036854775807.0, 9223372036854775807.0);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }
    else if (dataType == "float" || dataType == "double")
    {
        minValueEdit->setRange(-9999999.0, 9999999.0);
        maxValueEdit->setRange(-9999999.0, 9999999.0);
        initialValueEdit->setRange(-9999999.0, 9999999.0);
        minValueEdit->setDecimals(6);
        maxValueEdit->setDecimals(6);
        initialValueEdit->setDecimals(6);
    }
    else if (dataType == "bool")
    {
        minValueEdit->setRange(0, 1);
        maxValueEdit->setRange(0, 1);
        initialValueEdit->setRange(0, 1);
        minValueEdit->setDecimals(0);
        maxValueEdit->setDecimals(0);
        initialValueEdit->setDecimals(0);
    }

    double minVal = minValueEdit->value();
    double maxVal = maxValueEdit->value();
    double initialVal = initialValueEdit->value();

    if (minVal > maxVal) {
        minValueEdit->setValue(maxVal);
    }

    if (initialVal < minVal && initialValueEdit->minimum() != minValueEdit->minimum()) {
        initialValueEdit->setValue(initialVal);
    } else if (initialVal > maxVal) {
        initialValueEdit->setValue(maxVal);
    }

    if (callbackCheck->isChecked() && typeCombo->currentText() == tr("Changeable")) {
        functionParamsLabel->setText(tr("const %1 value - 当前值").arg(dataType));
    }

    emitModifiedData();
}

void MenuItemEditor::onDataChanged()
{
    emit dataChanged();
}

void MenuItemEditor::setupConnections()
{
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MenuItemEditor::onTypeChanged);

    connect(nameEdit, &QLineEdit::textChanged,
            this, &MenuItemEditor::emitModifiedData);
    connect(varNameEdit, &QLineEdit::textChanged,
            this, &MenuItemEditor::emitModifiedData);
    connect(funcNameEdit, &QLineEdit::textChanged,
            this, &MenuItemEditor::emitModifiedData);
    connect(argsNameEdit, &QLineEdit::textChanged,
            this, &MenuItemEditor::emitModifiedData);
    connect(funcNameEdit, &QLineEdit::textChanged,
            this, &MenuItemEditor::emitModifiedData);
    connect(argsNameEdit, &QLineEdit::textChanged,
            this, &MenuItemEditor::emitModifiedData);

    connect(minValueEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) {
                // 确保最大值不小于最小值
                if (maxValueEdit->value() < value) {
                    maxValueEdit->setValue(value);
                }
                // 确保初始值不小于最小值
                if (initialValueEdit->value() < value) {
                    initialValueEdit->setValue(value);
                }
            });

    connect(maxValueEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double value) {
                // 确保最小值不大于最大值
                if (minValueEdit->value() > value) {
                    minValueEdit->setValue(value);
                }
                // 确保初始值不大于最大值
                if (initialValueEdit->value() > value) {
                    initialValueEdit->setValue(value);
                }
            });
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MenuItemEditor::emitModifiedData);
    connect(dataTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MenuItemEditor::emitModifiedData);
    connect(initialValueEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MenuItemEditor::emitModifiedData);
    connect(minValueEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MenuItemEditor::emitModifiedData);
    connect(maxValueEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MenuItemEditor::emitModifiedData);
    connect(stepEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MenuItemEditor::emitModifiedData);
    connect(callbackCheck, &QCheckBox::toggled,
            this, &MenuItemEditor::onCallbackToggled);
    connect(callbackCheck, &QCheckBox::toggled,
            this, &MenuItemEditor::emitModifiedData);
    connect(callbackEdit, &QTextEdit::textChanged,
            this, &MenuItemEditor::emitModifiedData);

    connect(codePreviewButton, &QPushButton::clicked,
            this, &MenuItemEditor::onGenerateCodePreview);
    connect(saveCodeButton, &QPushButton::clicked,
            this, &MenuItemEditor::onSaveCodeToFile);
}

void MenuItemEditor::setItemData(const ItemData& data)
{
    // 阻止在设置数据时触发不必要的信号
    blockSignals(true);

    currentData = data; // 保存当前数据
    isRootNode = data.isRoot;

    // 如果是根节点，固定显示"Main Menu"，否则显示实际名称
    if (isRootNode)
    {
        nameEdit->setText("Main Menu");
    }
    else
    {
        nameEdit->setText(data.name);
    }
    varNameEdit->setText(data.varName);
    funcNameEdit->setText(data.funcName);
    argsNameEdit->setText(data.argsName);

    int typeIndex = typeCombo->findText(data.type);
    if (typeIndex >= 0)
    {
        typeCombo->setCurrentIndex(typeIndex);
    }

    // 根据类型正确设置回调复选框状态
    if (data.type == tr("Normal"))
    {
        callbackCheck->setChecked(false);
        callbackCheck->setEnabled(false);
    }
    else if (data.type == tr("Application"))
    {
        callbackCheck->setChecked(true);
        callbackCheck->setEnabled(false);
    }
    else
    {
        callbackCheck->setEnabled(true);
        callbackCheck->setChecked(data.hasCallback);
    }

    if (data.type == tr("Changeable"))
    {
        int dataTypeIndex = dataTypeCombo->findText(data.dataType);
        if (dataTypeIndex >= 0)
        {
            dataTypeCombo->setCurrentIndex(dataTypeIndex);
        }

        initialValueEdit->setValue(data.initialValue);
        minValueEdit->setValue(data.minValue);
        maxValueEdit->setValue(data.maxValue);
        stepEdit->setValue(data.step);
    }

    callbackCheck->setChecked(data.hasCallback);
    callbackEdit->setText(data.callbackCode);

    updateVisibility();
    updateEditability();

    blockSignals(false);

    // 发送初始数据
    emitModifiedData();
}

void MenuItemEditor::emitModifiedData()
{
    ItemData data = getItemData();
    currentData = data; // 更新当前数据
    emit itemModified(data); // 发送实时更新信号
    emit dataChanged(); // 保持原有的信号
}

void MenuItemEditor::updateEditability()
{
    if (isRootNode)
    {
        // 根节点所有属性都禁用编辑，包括回调
        nameEdit->setEnabled(false);
        varNameEdit->setEnabled(false);
        funcNameEdit->setEnabled(false);
        argsNameEdit->setEnabled(false);
        typeCombo->setEnabled(false);
        dataTypeCombo->setEnabled(false);
        initialValueEdit->setEnabled(false);
        minValueEdit->setEnabled(false);
        maxValueEdit->setEnabled(false);
        stepEdit->setEnabled(false);
        callbackCheck->setEnabled(false);
        callbackEdit->setEnabled(false);

        // 确保显示固定的根节点名称
        nameEdit->setText("Main Menu");
    }
    else
    {
        // 非根节点允许编辑所有属性
        nameEdit->setEnabled(true);
        varNameEdit->setEnabled(true);
        funcNameEdit->setEnabled(true);
        argsNameEdit->setEnabled(true);
        typeCombo->setEnabled(true);
        dataTypeCombo->setEnabled(true);
        initialValueEdit->setEnabled(true);
        minValueEdit->setEnabled(true);
        maxValueEdit->setEnabled(true);
        stepEdit->setEnabled(true);
        callbackCheck->setEnabled(true);
        callbackEdit->setEnabled(true);
    }
}

MenuItemEditor::ItemData MenuItemEditor::getItemData() const
{
    ItemData data = currentData; // 基于当前数据创建新的数据对象

    data.name = nameEdit->text();
    data.varName = varNameEdit->text();
    data.funcName = funcNameEdit->text();
    data.argsName = argsNameEdit->text();
    data.type = typeCombo->currentText();
    data.isRoot = isRootNode;

    if (data.type == tr("Changeable"))
    {
        data.dataType = dataTypeCombo->currentText();
        data.initialValue = initialValueEdit->value();
        data.minValue = minValueEdit->value();
        data.maxValue = maxValueEdit->value();
        data.step = stepEdit->value();
    }

    data.hasCallback = callbackCheck->isChecked() || data.type == tr("Application");
    data.callbackCode = callbackEdit->toPlainText();

    return data;
}

void MenuItemEditor::onGenerateCodePreview()
{
    // 获取当前编辑的数据
    ItemData data = getItemData();

    // 使用代码生成器生成预览
    QString previewCode = CodeGenerator::generateCodePreview(data);

    // 显示在预览文本框中
    codePreviewEdit->setText(previewCode);
}

void MenuItemEditor::onSaveCodeToFile()
{
    // 首先生成代码预览
    onGenerateCodePreview();

    // 获取要保存的代码
    QString codeToSave = codePreviewEdit->toPlainText();
    if (codeToSave.isEmpty())
    {
        QMessageBox::warning(this, tr("警告"), tr("没有生成任何代码可以保存！"));
        return;
    }

    // 弹出文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("保存代码文件"),
                                                    QString(),
                                                    tr("C++ 文件 (*.cpp *.h);;所有文件 (*)"));

    if (!filePath.isEmpty())
    {
        // 使用代码生成器保存代码到文件
        if (CodeGenerator::saveCodeToFile(filePath, codeToSave))
        {
            QMessageBox::information(this, tr("成功"), tr("代码已成功保存到文件！"));
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr("保存代码到文件失败！"));
        }
    }
}


void MenuItemEditor::updateVisibility()
{
    QString currentType = typeCombo->currentText();
    bool isChangeable = currentType == tr("Changeable");
    bool isApp = currentType == tr("Application");
    bool isNormal = currentType == tr("Normal");
    bool needsVarName = (currentType == tr("Changeable") || currentType == tr("bool"));

    changeableWidget->setVisible(isChangeable);
    appWidget->setVisible(isApp);
    varNameEdit->setVisible(needsVarName);
    varNameLabel->setVisible(needsVarName); // 直接使用保存的标签引用

    // 根据当前类型强制设置回调复选框的状态
    if (isNormal)
    {
        callbackCheck->setChecked(false);
        callbackCheck->setEnabled(false);
        callbackEdit->setVisible(false);
        functionParamsLabel->setVisible(false);
        callbackCheck->style()->polish(callbackCheck); // 强制立即更新样式
        callbackCheck->update(); // 强制重绘
    }
    else if (isApp)
    {
        callbackCheck->setChecked(true);
        callbackCheck->setEnabled(false);
        callbackEdit->setVisible(true);
        functionParamsLabel->setVisible(true);
    }
    else
    {
        callbackCheck->setEnabled(true);
        bool hasCallback = callbackCheck->isChecked();
        callbackEdit->setVisible(hasCallback);
        functionParamsLabel->setVisible(hasCallback);
    }
}

void MenuItemEditor::onCallbackToggled(bool checked)
{
    // 更新函数参数标签和提示文本
    QString currentType = typeCombo->currentText();

    if (checked)
    {
        if (currentType == tr("Changeable"))
        {
            QString dataType = dataTypeCombo->currentText();
            functionParamsLabel->setText(tr("const %1 value - 当前值").arg(dataType));
            callbackEdit->setPlaceholderText(tr("输入回调函数代码，可以通过value参数访问当前值"));
        }
        else if (currentType == tr("bool"))
        {
            functionParamsLabel->setText(tr("const bool state - 当前状态"));
            callbackEdit->setPlaceholderText(tr("输入回调函数代码，可以通过state参数访问当前状态"));
        }
        functionParamsLabel->setVisible(true);
    }
    else
    {
        functionParamsLabel->setVisible(false);
    }

    updateVisibility();
}


void MenuItemEditor::setupUI()
{
    auto* layout = new QFormLayout(this);

    // 基本属性
    nameEdit = new QLineEdit(this);
    layout->addRow(tr("名称:"), nameEdit);

    // 变量名控件和标签
    varNameLabel = new QLabel(tr("变量名:"), this);
    varNameEdit = new QLineEdit(this);
    layout->addRow(varNameLabel, varNameEdit);

    // 应用类型的专用控件
    appWidget = new QWidget(this);
    auto* appLayout = new QFormLayout(appWidget);

    funcNameEdit = new QLineEdit(appWidget);
    appLayout->addRow(tr("函数名:"), funcNameEdit);

    argsNameEdit = new QLineEdit(appWidget);
    appLayout->addRow(tr("参数名:"), argsNameEdit);

    layout->addRow(appWidget);
    appWidget->setVisible(false); // 初始不可见

    typeCombo = new QComboBox(this);
    typeCombo->addItems({tr("Normal"), tr("Changeable"), tr("bool"), tr("Application")});
    // 确保默认为普通类型
    typeCombo->setCurrentIndex(0); // 0 = 普通
    layout->addRow(tr("菜单项类型:"), typeCombo);

    // 可变类型属性
    changeableWidget = new QWidget(this);
    auto* changeableLayout = new QFormLayout(changeableWidget);

    dataTypeCombo = new QComboBox(changeableWidget);
    dataTypeCombo->addItems({"uint8_t", "uint16_t", "uint32_t", "uint64_t", "int8_t", "int16_t", "int32_t", "int64_t", "float", "double", "bool"});
    changeableLayout->addRow(tr("数据类型:"), dataTypeCombo);
    connect(dataTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MenuItemEditor::onDataTypeChanged);

    initialValueEdit = new QDoubleSpinBox(changeableWidget);
    initialValueEdit->setRange(-9999999, 9999999);
    initialValueEdit->setDecimals(0); // 默认不显示小数位
    initialValueEdit->setValue(0); // 设置默认初始值为0
    changeableLayout->addRow(tr("初始值:"), initialValueEdit);

    minValueEdit = new QDoubleSpinBox(changeableWidget);
    minValueEdit->setRange(-9999999, 9999999);
    minValueEdit->setDecimals(0); // 默认不显示小数位
    changeableLayout->addRow(tr("最小值:"), minValueEdit);

    maxValueEdit = new QDoubleSpinBox(changeableWidget);
    maxValueEdit->setRange(-9999999, 9999999);
    maxValueEdit->setDecimals(0); // 默认不显示小数位
    changeableLayout->addRow(tr("最大值:"), maxValueEdit);

    stepEdit = new QDoubleSpinBox(changeableWidget);
    stepEdit->setRange(0.0, 9999999);
    stepEdit->setDecimals(2); // 步进值允许小数
    stepEdit->setValue(1); // 设置默认步进值为1
    changeableLayout->addRow(tr("步进值:"), stepEdit);

    layout->addRow(changeableWidget);

    // 回调设置
    callbackCheck = new QCheckBox(tr("启用回调"), this);
    // 默认为普通类型，禁用回调复选框
    callbackCheck->setEnabled(false);
    callbackCheck->setChecked(false);
    // 立即强制应用禁用样式
    callbackCheck->setStyleSheet("QCheckBox { color: gray; }");
    callbackCheck->style()->unpolish(callbackCheck);
    callbackCheck->style()->polish(callbackCheck);
    callbackCheck->update(); // 强制重绘
    layout->addRow(callbackCheck);

    callbackEdit = new QTextEdit(this);
    callbackEdit->setPlaceholderText(tr("输入回调函数代码"));
    layout->addRow(callbackEdit);

    // 添加函数参数提示标签
    functionParamsLabel = new QLabel(this);
    functionParamsLabel->setVisible(false);
    layout->addRow(tr("函数参数:"), functionParamsLabel);

    // 添加代码预览按钮和代码预览文本框
    codePreviewButton = new QPushButton(tr("生成代码预览"), this);
    layout->addRow(codePreviewButton);

    codePreviewEdit = new QTextEdit(this);
    codePreviewEdit->setReadOnly(true);
    codePreviewEdit->setPlaceholderText(tr("代码预览将显示在这里"));
    layout->addRow(codePreviewEdit);

    // 保存代码到文件按钮
    saveCodeButton = new QPushButton(tr("保存代码到文件"), this);
    layout->addRow(saveCodeButton);

    // 由于默认是普通类型，确保回调选项禁用
    callbackCheck->setChecked(false);
    callbackCheck->setEnabled(false);
    callbackEdit->setVisible(false);
    functionParamsLabel->setVisible(false);

    // 更新所有组件的可见性
    // 更新UI组件可见性
    updateVisibility();

    // 确保回调编辑框在初始状态下是隐藏的
    callbackEdit->setVisible(false);
    functionParamsLabel->setVisible(false);
}

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include "numbar.h"

NumBar::NumBar(const QString &title, int min, int max, QWidget *parent)
    : QWidget(parent) {
    // 创建水平布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout -> setContentsMargins(5, 5, 5, 5);
    layout -> setSpacing(5);

    // 添加标题标签
    QLabel *titleLabel = new QLabel(title, this);
    titleLabel -> setStyleSheet("font-size: 20px; font-weight: bold; color: #2d3748; text-align: center;");
    titleLabel->setMinimumWidth(150); // 固定标题宽度
    layout->addWidget(titleLabel);

    // 创建输入框
    spinBox = new QSpinBox(this);
    spinBox->setStyleSheet(
        "QSpinBox {"
        "    border: 2px solid #000000;"
        "    border-radius: 4px;"
        "    padding: 2px;"
        "    font-size: 20px"
        "}"
        );
    spinBox->setRange(min, max);
    spinBox->setValue(min);
    spinBox->setFixedWidth(100);
    layout->addWidget(spinBox);

    // 创建滑块
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min, max);
    slider->setValue(min);
    slider->setMinimumHeight(30);
    slider->setMaximumHeight(30);
    slider->setMinimumWidth(100); // 最小宽度
    slider->setMaximumWidth(300); // 最大宽度
    layout->addWidget(slider, 1); // 可拉伸

    // 双向绑定
    connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            slider, &QSlider::setValue);

    connect(slider, &QSlider::valueChanged,
            spinBox, &QSpinBox::setValue);
}

int NumBar::value() const{
    return spinBox->value();
}
void NumBar::setValue(int value) {
    spinBox->setValue(value);
}

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>

class NumBar : public QWidget {
    Q_OBJECT
public:
    NumBar(const QString &title, int min, int max, QWidget *parent = nullptr);

    // 获取当前值
    int value() const;

    // 设置当前值
    void setValue(int value);

private:
    QSlider *slider;
    QSpinBox *spinBox;
};

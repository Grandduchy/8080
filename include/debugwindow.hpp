#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <QWidget>

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = nullptr);
    ~DebugWindow();

private:
    Ui::DebugWindow *ui;
};

#endif // DEBUG_HPP

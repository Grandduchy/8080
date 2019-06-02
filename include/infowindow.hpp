#ifndef INFOWINDOW_HPP
#define INFOWINDOW_HPP

#include <QWidget>

namespace Ui {
class InfoWindow;
}

class InfoWindow : public QWidget {
    Q_OBJECT

public:
    explicit InfoWindow(QWidget *parent = nullptr);
    ~InfoWindow();

private:
    Ui::InfoWindow *ui;
};

#endif // INFOWINDOW_HPP

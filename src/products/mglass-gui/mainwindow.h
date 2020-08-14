#ifndef MGLASS_GUI_MAINWINDOW_H
#define MGLASS_GUI_MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private:
    Ui::MainWindow* ui_;
};

#endif // ndef MGLASS_GUI_MAINWINDOW_H
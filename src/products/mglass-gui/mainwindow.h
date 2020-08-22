#ifndef MGLASS_GUI_MAINWINDOW_H
#define MGLASS_GUI_MAINWINDOW_H

#include <QMainWindow>


namespace Ui
{
    class MainWindow;
}

class ImageView;


class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private slots: // QActions slots
    void onActionOpenTriggered();
    //void onActionQuitTriggered();
     void onActionHelpTriggered();

private slots: // shape selectors slots
    void onEllipseShapeSelectorTriggered();
    void onRectShapeSelectorTriggered();

private slots: // dx, dy slots
    void onDxValueChanged(double newValue);
    void onDyValueChanged(double newValue);

private slots: // options slots
    void onScaleFactorValueChanged(double newValue);
    void onAntiAliasingOptionStateChanged(int newState);
    void onAlphaBlendingOptionStateChanged(int newState);

private:
    Ui::MainWindow* ui_;
    ImageView* imageView_;
};

#endif // ndef MGLASS_GUI_MAINWINDOW_H

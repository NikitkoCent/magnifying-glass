#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageview.h"
#include <QObject>
#include <QAction>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QDebug>
#include <exception>        // std::exception


MainWindow::MainWindow()
    : ui_(new Ui::MainWindow())
    , imageView_(nullptr)
{
    ui_->setupUi(this);

    imageView_ = new ImageView(ui_->scrollArea);
    ui_->scrollArea->setWidget(imageView_);
    ui_->scrollAreaContainerWidget = ui_->scrollArea->widget();

    { // QActions bindings
        QObject::connect(ui_->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpenTriggered);
        QObject::connect(ui_->actionQuit, &QAction::triggered, this, &MainWindow::close);
        QObject::connect(ui_->actionShowUsageHelp, &QAction::triggered, this, &MainWindow::onActionHelpTriggered);
    }

    { // Shape selectors bindings
        QObject::connect(ui_->ellipseShapeButton, &QToolButton::triggered, this, &MainWindow::onEllipseShapeSelectorTriggered);
        QObject::connect(ui_->rectShapeButton, &QToolButton::triggered, this, &MainWindow::onRectShapeSelectorTriggered);
    }

    { // dx, dy bindings
        QObject::connect(ui_->dxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onDxValueChanged);
        QObject::connect(ui_->dySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onDyValueChanged);
    }

    { // options bindings
        QObject::connect(ui_->scaleFactorSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::onScaleFactorValueChanged);
        QObject::connect(ui_->interpolateCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onInterpolateOptionStateChanged);
        QObject::connect(ui_->alphaBlendingCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onAlphaBlendingOptionStateChanged);
    }
}

MainWindow::~MainWindow()
{
    delete ui_;
    ui_ = nullptr;
}


void MainWindow::onActionOpenTriggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), {}, tr("Image Files (*.png)"));

    while (!fileName.isNull())
    {
        try
        {
            imageView_->setImage(mglass::Image::fromPNGFile(fileName.toStdString()));
            break;
        }
        catch (const std::exception& err)
        {
            QMessageBox::critical(this, "Failed to load the image", QString::fromUtf8(err.what()));
        }
        catch (...)
        {
            QMessageBox::critical(this, "Failed to load the image", tr("Unknown error"));
        }

        fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), {}, tr("Image Files (*.png)"));
    }
}

void MainWindow::onActionHelpTriggered()
{
    return (void)QMessageBox::information(this, tr("Usage help"),
        tr("Open PNG image via File->Open (Ctrl+O).\n"
           "Then move mouse cursor to the area of displayed image and press any mouse button."));
}


void MainWindow::onEllipseShapeSelectorTriggered()
{
    // TODO: implement
}

void MainWindow::onRectShapeSelectorTriggered()
{
    // TODO: implement
}


void MainWindow::onDxValueChanged(double newValue)
{
    qDebug() << newValue;
}

void MainWindow::onDyValueChanged(double newValue)
{
    qDebug() << newValue;
}


void MainWindow::onScaleFactorValueChanged(double newValue)
{
    qDebug() << newValue;

    try
    {
        imageView_->setScaleFactor(static_cast<mglass::float_type>(newValue));
    }
    catch (const std::exception& err)
    {

    }
    catch (...)
    {

    }
}


void MainWindow::onInterpolateOptionStateChanged(int newState)
{
    switch (newState)
    {
        case Qt::Checked:
            imageView_->enableInterpolating();
            break;
        default:
            imageView_->disableInterpolating();
            break;
    }
}

void MainWindow::onAlphaBlendingOptionStateChanged(int newState)
{
    switch (newState)
    {
        case Qt::Checked:
            imageView_->enableAntiAliasing();
            break;
        default:
            imageView_->disableAntiAliasing();
            break;
    }
}

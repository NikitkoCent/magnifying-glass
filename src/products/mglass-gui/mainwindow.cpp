#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageview.h"
#include "mglass-extensions/polymorphic_shapes.h"   // mglassext::Polymorphic*
#include <QObject>
#include <QAction>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QDebug>
#include <exception>                                // std::exception


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
        QObject::connect(ui_->ellipseShapeButton, &QToolButton::clicked, this, &MainWindow::onEllipseShapeSelectorTriggered);
        QObject::connect(ui_->rectShapeButton, &QToolButton::clicked, this, &MainWindow::onRectShapeSelectorTriggered);
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

    // set default shape
    onEllipseShapeSelectorTriggered();
}

MainWindow::~MainWindow()
{
    delete ui_;
    ui_ = nullptr;
}


void MainWindow::onActionOpenTriggered()
{
    qDebug() << __func__;

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
    qDebug() << __func__;

    return (void)QMessageBox::information(this, tr("Usage help"),
        tr("Open PNG image via File->Open (Ctrl+O).\n"
           "Then move mouse cursor to the area of displayed image and press any mouse button."));
}


void MainWindow::onEllipseShapeSelectorTriggered()
{
    qDebug() << __func__;

    imageView_->setShape(std::make_unique<mglassext::PolymorphicEllipse>(
        static_cast<mglass::float_type>(ui_->dxSpinBox->value()),
        static_cast<mglass::float_type>(ui_->dySpinBox->value())
    ));
}

void MainWindow::onRectShapeSelectorTriggered()
{
    qDebug() << __func__;

    imageView_->setShape(std::make_unique<mglassext::PolymorphicRectangle>(
        static_cast<mglass::float_type>(ui_->dxSpinBox->value()),
        static_cast<mglass::float_type>(ui_->dySpinBox->value())
    ));
}


void MainWindow::onDxValueChanged(double newValue)
{
    qDebug() << __func__ << newValue;

    if (ui_->ellipseShapeButton->isChecked())
        imageView_->setShape(std::make_unique<mglassext::PolymorphicEllipse>(
            static_cast<mglass::float_type>(newValue),
            static_cast<mglass::float_type>(ui_->dySpinBox->value())
        ));
    else
        imageView_->setShape(std::make_unique<mglassext::PolymorphicRectangle>(
            static_cast<mglass::float_type>(newValue),
            static_cast<mglass::float_type>(ui_->dySpinBox->value())
        ));
}

void MainWindow::onDyValueChanged(double newValue)
{
    qDebug() << __func__ << newValue;

    if (ui_->ellipseShapeButton->isChecked())
        imageView_->setShape(std::make_unique<mglassext::PolymorphicEllipse>(
            static_cast<mglass::float_type>(ui_->dxSpinBox->value()),
            static_cast<mglass::float_type>(newValue)
        ));
    else
        imageView_->setShape(std::make_unique<mglassext::PolymorphicRectangle>(
            static_cast<mglass::float_type>(ui_->dxSpinBox->value()),
            static_cast<mglass::float_type>(newValue)
        ));
}


void MainWindow::onScaleFactorValueChanged(double newValue)
{
    qDebug() << __func__ << newValue;

    imageView_->setScaleFactor(static_cast<mglass::float_type>(newValue));
}


void MainWindow::onInterpolateOptionStateChanged(int newState)
{
    qDebug() << __func__ << newState;

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
    qDebug() << __func__ << newState;

    switch (newState)
    {
        case Qt::Checked:
            imageView_->enableAlphaBlending();
            break;
        default:
            imageView_->disableAlphaBlending();
            break;
    }
}

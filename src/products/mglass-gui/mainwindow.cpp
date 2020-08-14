#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mglass/mglass.h"
#include "mglass/shapes.h"
#include "mglass/magnifiers.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QObject>
#include <QMouseEvent>
#include <QDebug>


namespace
{
    class ImageView final : public QWidget
    {
        Q_OBJECT

    public:
        explicit ImageView(QWidget* parent = nullptr)
            : QWidget(parent)
            , layout_(new QVBoxLayout(this))
            , imageLabel_(new QLabel(this))
            , cursor_(cursor())
        {
            setStyleSheet("background-color:gray;");

            layout_->setContentsMargins(imageMargins_, imageMargins_, imageMargins_, imageMargins_);
            layout_->setAlignment(Qt::AlignTop);
            layout_->addWidget(imageLabel_);

            const QString imgPath = "D:\\Download\\Lenna.png";

            mglassImg_ = mglass::Image::fromPNGFile(imgPath.toStdString());
            imageLabel_->setPixmap(QPixmap(imgPath));
        }

    private:
        void mouseMoveEvent(QMouseEvent* e) override
        {
            QWidget::mouseMoveEvent(e);

            const auto pos = e->pos();
            const auto x = pos.x();
            const auto y = pos.y();

            qDebug() << x << ' ' << y;

            if ( (x < 0) || (x >= width()) )
                return;
            if ( (y < 0) || (y >= height()) )
                return;

            const auto xFloat = static_cast<mglass::float_type>(x);
            const auto yFloat = static_cast<mglass::float_type>(y);

            const mglass::shapes::Ellipse shape({ xFloat, -yFloat }, 350, 150);

            const mglass::Point<mglass::int_type> mglassImgPos{ imageLabel_->pos().x(), -imageLabel_->pos().y() };
            mglass::magnifiers::nearestNeighbor(shape, 2, mglassImg_, mglassImgPos, magnifiedImg_);

            QImage newCursorImg(magnifiedImg_.getWidth(), magnifiedImg_.getHeight(), QImage::Format_ARGB32);
            for (mglass::size_type y = 0; y < magnifiedImg_.getHeight(); ++y)
            {
                for (mglass::size_type x = 0; x < magnifiedImg_.getWidth(); ++x)
                {
                    const auto pixel = magnifiedImg_.getPixelAt(x, y);
                    newCursorImg.setPixelColor(x, y, QColor(pixel.r, pixel.g, pixel.b, pixel.a));
                }
            }

            setCursor(QPixmap::fromImage(newCursorImg));
        }

        void mousePressEvent(QMouseEvent* e) override
        {
            cursor_ = cursor();
            setCursor(Qt::PointingHandCursor);

            QWidget::mousePressEvent(e);
        }

        void mouseReleaseEvent(QMouseEvent* e) override
        {
            setCursor(cursor_);

            QWidget::mouseReleaseEvent(e);
        }

    private:
        static constexpr int imageMargins_ = 50;

        QVBoxLayout* layout_;
        QLabel* imageLabel_;
        QCursor cursor_;
        mglass::Image mglassImg_;
        mglass::Image magnifiedImg_;
    };
}


MainWindow::MainWindow()
    : ui_(new Ui::MainWindow())
{
    ui_->setupUi(this);

    ui_->scrollArea->setWidget(new ImageView());
}

MainWindow::~MainWindow()
{
    delete ui_;
    ui_ = nullptr;
}

#include "mainwindow.moc"

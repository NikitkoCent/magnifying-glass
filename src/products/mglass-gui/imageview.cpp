#include "imageview.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <utility>              // std::move
#include <cmath>                // std::isnan
#include <stdexcept>            // std::invalid_argument


ImageView::ImageView(QWidget* parent)
    : QWidget(parent)
    , scaleFactor_(2.0)
    , alphaBlendingIsEnabled_(false)
    , interpolatingIsEnabled_(false)
    , magnifierIsDisplayed_(false)
    , layout_(new QVBoxLayout(this))
    , imageLabel_(new QLabel(tr("Use File->Open (Ctrl+O) to load an image"), this))
    , defaultCursor_(cursor())
{
    setStyleSheet("background-color:gray;");

    layout_->setContentsMargins(imageAreaMargins_, imageAreaMargins_, imageAreaMargins_, imageAreaMargins_);
    layout_->addWidget(imageLabel_, Qt::AlignTop);

    imageLabel_->setAlignment(Qt::AlignCenter);
}


void ImageView::setImage(mglass::Image&& newImg)
{
    mglassWholeImg_ = std::move(newImg);

    QImage imgBuf;
    convertMglassImgToQtImg(*mglassWholeImg_, imgBuf);

    imageLabel_->setPixmap(QPixmap::fromImage(std::move(imgBuf)));
    imageLabel_->setAlignment(Qt::AlignLeft);
    imageLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    imageLabel_->setStyleSheet("QLabel {"
                                "border-style: solid;"
                                "border-width: " + QString::number(imageBorders_) + "px;"
                                "border-color: black; "
                                "}");

    updateMagnifierCursor();
}


void ImageView::setShape(std::unique_ptr<mglassext::PolymorphicShape> newShape) noexcept(false)
{
    if (!newShape)
        throw std::invalid_argument("`newShape` does not own an object");

    mglassShape_ = std::move(newShape);

    updateMagnifierCursor();
}


void ImageView::setScaleFactor(mglass::float_type newScaleFactor) noexcept(false)
{
    if (std::isnan(newScaleFactor))
        throw std::invalid_argument("ImageView::setScaleFactor(float newScaleFactor): newScaleFactor is NaN");

    if (std::isinf(newScaleFactor) || (newScaleFactor <= 0))
        throw std::invalid_argument("ImageView::setScaleFactor(float newScaleFactor): newScaleFactor is not inside the range (0; +inf)");

    scaleFactor_ = newScaleFactor;

    updateMagnifierCursor();
}


void ImageView::enableAlphaBlending()
{
    alphaBlendingIsEnabled_ = true;

    updateMagnifierCursor();
}

void ImageView::disableAlphaBlending()
{
    alphaBlendingIsEnabled_ = false;

    updateMagnifierCursor();
}


void ImageView::enableInterpolating()
{
    interpolatingIsEnabled_ = true;

    updateMagnifierCursor();
}

void ImageView::disableInterpolating()
{
    interpolatingIsEnabled_ = false;

    updateMagnifierCursor();
}


void ImageView::mouseMoveEvent(QMouseEvent* e)
{
    QWidget::mouseMoveEvent(e);

    if (magnifierIsDisplayed_)
    {
        const auto pos = e->pos();
        updateMagnifierCursor(pos.x(), pos.y());
    }
}

void ImageView::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent(e);

    if (!magnifierIsDisplayed_)
        enableMagnifier();

    const auto pos = e->pos();
    updateMagnifierCursor(pos.x(), pos.y());
}

void ImageView::mouseReleaseEvent(QMouseEvent* e)
{
    QWidget::mouseReleaseEvent(e);

    if (magnifierIsDisplayed_)
    {
        disableMagnifier();

        const auto pos = e->pos();
        updateMagnifierCursor(pos.x(), pos.y());
    }
}


void ImageView::enableMagnifier()
{
    if (mglassWholeImg_.has_value())
    {
        magnifierIsDisplayed_ = true;
        defaultCursor_ = cursor();
    }
}

void ImageView::disableMagnifier()
{
    magnifierIsDisplayed_ = false;
    setCursor(defaultCursor_);
}


void ImageView::updateMagnifierCursor(int posX, int posY)
{
    if (!magnifierIsDisplayed_)
        return;

    if (!mglassShape_)
        return (void)disableMagnifier();

    if (!visibleRegion().contains(QPoint(posX, posY)))
        return (void)disableMagnifier();

    const auto xFloat = static_cast<mglass::float_type>(posX);
    const auto yFloat = static_cast<mglass::float_type>(posY);

    mglassShape_->moveCenterTo(xFloat, -yFloat);

    const mglass::Point<mglass::int_type> mglassImgPos{ imageLabel_->pos().x() + imageBorders_, -imageLabel_->pos().y() - imageBorders_ };

    // TODO: crop mglassWholeImg_ to visibleRegion(). It can be implemented via smth like mglass::ImageView

    if (interpolatingIsEnabled_)
        mglassShape_->applyNearestNeighborAntiAliased(scaleFactor_, *mglassWholeImg_, mglassImgPos, mglassMagnifiedImg_, alphaBlendingIsEnabled_);
    else
        mglassShape_->applyNearestNeighbor(scaleFactor_, *mglassWholeImg_, mglassImgPos, mglassMagnifiedImg_, alphaBlendingIsEnabled_);

    convertMglassImgToQtImg(mglassMagnifiedImg_, cursorImgBuf_);

    setCursor(QPixmap::fromImage(cursorImgBuf_));
}

void ImageView::updateMagnifierCursor()
{
    if (!magnifierIsDisplayed_)
        return;

    const auto pos = mapFromGlobal(cursor().pos());
    updateMagnifierCursor(pos.x(), pos.y());
}


void ImageView::convertMglassImgToQtImg(const mglass::Image& src, QImage& dst)
{
    const mglass::size_type width = src.getWidth();
    const mglass::size_type height = src.getHeight();

    const auto widthSigned = static_cast<int>(width);
    const auto heightSigned = static_cast<int>(height);

    // I did not find any effective way for resizing QImage in-place.
    //
    // Unless we can pass custom raw buffer to QImage,
    //  but it will be really hard to track the lifetimes of all CoW-copies of QImage use this buffer.
    dst = QImage(widthSigned, heightSigned, QImage::Format_ARGB32);

    mglass::size_type y = 0;
    for (int ySigned = 0; ySigned < heightSigned; ++ySigned, ++y)
    {
        mglass::size_type x = 0;
        for (int xSigned = 0; xSigned < widthSigned; ++xSigned, ++x)
        {
            const auto pixel = src.getPixelAt(x, y);
            dst.setPixelColor(xSigned, ySigned, QColor(pixel.r, pixel.g, pixel.b, pixel.a));
        }
    }
}

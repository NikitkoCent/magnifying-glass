#ifndef MGLASS_GUI_IMAGEVIEW_H
#define MGLASS_GUI_IMAGEVIEW_H

#include "polymorphic_shape.h"  // PolymorphicShape
#include "mglass/mglass.h"      // mglass::*
#include <QWidget>
#include <QCursor>
#include <QImage>
#include <memory>               // std::unique_ptr
#include <optional>             // std::optional


class QVBoxLayout;
class QLabel;
class QMouseEvent;


// TODO: add description
class ImageView final : public QWidget
{
    Q_OBJECT

public: // ctors/dtor
    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() = default;

public: // modifiers
    void setImage(mglass::Image&& newImg);

    // throws std::invalid_argument if `newShape` does not own an object (!newShape returns true)
    void setShape(std::unique_ptr<PolymorphicShape> newShape) noexcept(false);

    // throws std::invalid_argument exception if newScaleFactor is NaN or is not inside the range (0; +inf)
    void setScaleFactor(mglass::float_type newScaleFactor) noexcept(false);

    void enableAntiAliasing();
    void disableAntiAliasing();

    void enableInterpolating();
    void disableInterpolating();

public: // getters
    mglass::float_type getScaleFactor() const noexcept { return scaleFactor_; }
    bool isAntiAliasingEnabled() const noexcept { return antialiasingIsEnabled_; }
    bool isInterpolatingEnabled() const noexcept { return interpolatingIsEnabled_; }

private: // mouse events handlers
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private: // enable/disable displaying of the magnifier glass
    void enableMagnifier();
    void disableMagnifier();

private:
    void updateMagnifierCursor(int posX, int posY);
    void updateMagnifierCursor();

private:
    static void convertMglassImgToQtImg(const mglass::Image& src, QImage& dst);

private:
    static constexpr int imageAreaMargins_ = 50;
    static constexpr int imageBorders_ = 5;

private:
    std::unique_ptr<PolymorphicShape> mglassShape_;
    mglass::float_type scaleFactor_;
    bool antialiasingIsEnabled_;
    bool interpolatingIsEnabled_;
    bool magnifierIsDisplayed_;
    QVBoxLayout* layout_;
    QLabel* imageLabel_;
    QCursor defaultCursor_;
    std::optional<mglass::Image> mglassWholeImg_;
    mglass::Image mglassMagnifiedImg_;
    QImage cursorImgBuf_;
};

#endif // ndef MGLASS_GUI_IMAGEVIEW_H

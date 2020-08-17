#ifndef MGLASS_GUI_IMAGEVIEW_H
#define MGLASS_GUI_IMAGEVIEW_H

#include "mglass/mglass.h"  // mglass::Image
#include "mglass/shapes.h"  // mglass::shapes::*
#include <QWidget>
#include <QCursor>
#include <QImage>
#include <variant>          // std::variant
#include <optional>         // std::optional


class QVBoxLayout;
class QLabel;
class QMouseEvent;


class ImageView final : public QWidget
{
    Q_OBJECT

public: // ctors/dtor
    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() = default;

public: // modifiers
    void setImage(mglass::Image&& newImg);

    // TODO: move to CRTP
    void setShape(mglass::shapes::Ellipse&& newShape);
    void setShape(mglass::shapes::Rectangle&& newShape);

    // throws std::invalid_argument exception if newScaleFactor is NaN or is not inside a range (0; +inf)
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
    // we also can move to some abstract interface with pure-virtual methods
    // TODO: uncomment second param
    std::variant<mglass::shapes::Ellipse/*, mglass::shapes::Rectangle*/> mglassShape_;
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

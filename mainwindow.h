#pragma once

#include <QMainWindow>
#include <QImage>
#include <QDebug>
#include "Operation.hpp"
#include "State.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QScrollArea;
class QLabel;
class QVBoxLayout;
class QScrollBar;
QT_END_NAMESPACE

class QImageWrapper;
class StateMachine;
namespace image_transforming
{
class ImageTransformer;
} // namespace image_transforming
class ImageLabel;
class Panel;
class ChooseMaskDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    image_transforming::Operation operation{image_transforming::Operation::none};
    std::unique_ptr<image_transforming::ImageTransformer> imageTransformer;
    std::unique_ptr<QImageWrapper> imageWrapper;
    QImage image;
    ImageLabel* imageLabel;
    Panel* panel;
    ChooseMaskDialog* chooseMaskDialog;
    QScrollArea* scrollArea;
    QVBoxLayout* layout;
    QWidget* centralWidget;
    std::unique_ptr<StateMachine> stateMachine;

    float scaleFactor{1.0f};
    bool isTopTrimmed{false};

    static constexpr float maxScaleFactor{5.0f};
    static constexpr float minScaleFactor{0.25f};
    static constexpr float scaleFactorStep{0.1f};

    void enableMenuActions();
    void disableMenuActions();
    void toggleMenuActions(bool isEnabled);
    void askForSaveImage();
    void saveImage();
    void closeImage();
    void scaleImage(float factor);
    void setTopTrimmed(bool topTrimmed);
    void adjustScrollBar(QScrollBar* scrollBar, float factor);
    void rotate90();
    void drawImage();
    void refreshImage();
    void setShortcuts();
    void connectSignals();
    State createState();
    void updateState();
    void restoreState(State&);
    void resetOperation();

public slots:
    void on_actionEnablePanelToggled(bool);
    void on_actionOpenTriggered(bool);
    void on_actionSaveTriggered(bool);
    void on_actionCloseTriggered(bool);
    void on_actionExitTriggered(bool);
    void on_actionUndoTriggered(bool);
    void on_actionRedoTriggered(bool);
    void on_actionRotate90Triggered(bool);
    void on_actionApplyFilterTriggered(bool);
    void on_actionBackgroundRemovalTriggered(bool);
    void on_actionTrimTopTriggered(bool);

    void on_actionGpuAccelerationToggled(bool);
    void on_mouseWheelUsed(QPoint angleDelta);
    void on_mousePressedMoved(int x, int y);
    void on_mouseMoved(int x, int y);

    void on_sliderGainChanged(int, int, double, double);
    void on_sliderRangeChanged(int, int);
    void on_buttonResetClicked();
    void on_buttonRotateClicked();
    void on_rbEqualizeHistChecked();
    void on_rbGainChecked();
    void on_buttonApplyClicked();
    void on_buttonCancelClicked();
};

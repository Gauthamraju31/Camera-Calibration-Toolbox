#ifndef CAMERACALIBRATOR_H
#define CAMERACALIBRATOR_H

#include <QWidget>
#include <iostream>
#include <QSignalMapper>

enum WindowID
{
    MainWindow = 0,
    SingleCameraWindow = 1,
    StereoCameraWindow = 2,
    ErrorcheckWindow = 3,
    SingleCameraCalibratorWindow = 4,
    StereoCameraCalibratorWindow = 5
};

namespace Ui {
class CameraCalibrator;
}

class CameraCalibrator : public QWidget
{
    Q_OBJECT

public:
    explicit CameraCalibrator(QWidget *parent = nullptr);
    ~CameraCalibrator();

private slots:
    void handleButton();
    void next();
    void back();

private:
    Ui::CameraCalibrator *ui;
    int m_buttonID;
    int m_windowID;

};

#endif // CAMERACALIBRATOR_H

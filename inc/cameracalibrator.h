#ifndef CAMERACALIBRATOR_H
#define CAMERACALIBRATOR_H

#include <QWidget>
#include <iostream>
#include <QSignalMapper>

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

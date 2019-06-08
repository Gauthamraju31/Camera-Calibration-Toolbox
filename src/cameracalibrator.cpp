#include "cameracalibrator.h"
#include "ui_cameracalibrator.h"
#include "ui_selectsinglecamera.h"

//#define MainWindow 0
//#define SelectSingleCameraWindow 1
//#define SelectStereoCameraWindow 2
//#define ErrorCheckWindow 3
//#define SingleCameraCalibratorWindow 4
//#define StereoCamera CalibratorWindow 5

enum WindowIDS
{
    MainWindow = 0,
    SingleCameraWindow = 1,
    StereoCameraWindow = 2,
    ErrorcheckWindow = 3,
    SingleCameraCalibratorWindow = 4,
    StereoCameraCalibratorWindow = 5
};

CameraCalibrator::CameraCalibrator(QWidget *parent) :
    QWidget(parent), m_buttonID(-1), m_windowID(MainWindow),
    ui(new Ui::CameraCalibrator)
{
    ui->setupUi(this);
    ui->SingleCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
    ui->StereoCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
    ui->CalibErrorTesting_button->setStyleSheet(" QPushButton{ background-color:white }");
    ui->Back_button->setDisabled(true);
    ui->Next_button->setDisabled(true);
    connect(ui->SingleCalib_button, SIGNAL(released()), this, SLOT(handleButton()));
    connect(ui->StereoCalib_button, SIGNAL(released()), this, SLOT(handleButton()));
    connect(ui->CalibErrorTesting_button, SIGNAL(released()), this, SLOT(handleButton()));
    connect(ui->Next_button, SIGNAL(released()), this, SLOT(next()));
    connect(ui->Exit_button, SIGNAL(released()), this, SLOT(close()));

}

CameraCalibrator::~CameraCalibrator()
{
    delete ui;
}

void CameraCalibrator::handleButton()
{
    QObject* button = sender();
    if (button == ui->SingleCalib_button)
    {
        ui->SingleCalib_button->setStyleSheet(" QPushButton{ background-color:blue }");
        ui->StereoCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->CalibErrorTesting_button->setStyleSheet(" QPushButton{ background-color:white }");
        m_buttonID = SelectSingleCameraWindow;
    }
    else if (button == ui->StereoCalib_button)
    {
        ui->SingleCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->StereoCalib_button->setStyleSheet(" QPushButton{ background-color:blue }");
        ui->CalibErrorTesting_button->setStyleSheet(" QPushButton{ background-color:white }");
        m_buttonID = SelectStereoCameraWindow;
    }
    else if (button == ui->CalibErrorTesting_button)
    {
        ui->SingleCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->StereoCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->CalibErrorTesting_button->setStyleSheet(" QPushButton{ background-color:blue }");
        m_buttonID = ErrorCheckWindow;
    }
    else
    {
        std::cout << "Button Unknown" << std::endl;
    }
    ui->Next_button->setEnabled(true);
    QApplication::processEvents();
}

void CameraCalibrator::next()
{
    ui->stackedWidget->setCurrentIndex(1);

}

void CameraCalibrator::back()
{

}

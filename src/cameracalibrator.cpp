#include "cameracalibrator.h"
#include "ui_cameracalibrator.h"
#include "livecalibrationwindow.h"

CameraCalibrator::CameraCalibrator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraCalibrator),
    m_buttonID(-1),
    m_windowID(MainWindow)
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
    connect(ui->Back_button, SIGNAL(released()), this, SLOT(back()));
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
        m_buttonID = SingleCameraWindow;
    }
    else if (button == ui->StereoCalib_button)
    {
        ui->SingleCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->StereoCalib_button->setStyleSheet(" QPushButton{ background-color:blue }");
        ui->CalibErrorTesting_button->setStyleSheet(" QPushButton{ background-color:white }");
        m_buttonID = StereoCameraWindow;
    }
    else if (button == ui->CalibErrorTesting_button)
    {
        ui->SingleCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->StereoCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->CalibErrorTesting_button->setStyleSheet(" QPushButton{ background-color:blue }");
        m_buttonID = ErrorcheckWindow;
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
    if (m_windowID == MainWindow)
    {
        ui->stackedWidget->setCurrentIndex(m_buttonID);
        m_windowID = m_buttonID;
        ui->Back_button->setEnabled(true);
    }
    else if (m_windowID == SingleCameraWindow)
    {
        // Open the live calibration window
        LiveCalibrationWindow *liveCalib = new LiveCalibrationWindow();
        liveCalib->setWindowTitle("Single Camera Live Calibration");
        liveCalib->show();
        ui->statusLabel->setText("Live calibration window opened");
    }
    else std::cout<<"Not Implemented "<<m_buttonID<<std::endl;
}

void CameraCalibrator::back()
{
    if (m_windowID == SingleCameraWindow || m_windowID == StereoCameraWindow || m_windowID == ErrorcheckWindow)
    {
        ui->stackedWidget->setCurrentIndex(MainWindow);
        m_windowID = MainWindow;
        m_buttonID = MainWindow;
        ui->SingleCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->StereoCalib_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->CalibErrorTesting_button->setStyleSheet(" QPushButton{ background-color:white }");
        ui->Next_button->setDisabled(true);
        ui->Back_button->setDisabled(true);
    }
}

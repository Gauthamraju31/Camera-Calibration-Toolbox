#ifndef LIVECALIBRATIONWINDOW_H
#define LIVECALIBRATIONWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QImage>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

namespace Ui {
class LiveCalibrationWindow;
}

class LiveCalibrationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LiveCalibrationWindow(QWidget *parent = nullptr);
    ~LiveCalibrationWindow();

private slots:
    void onStartCamera();
    void onStopCamera();
    void onCaptureFrame();
    void onResetCalibration();
    void updateFrame();

private:
    Ui::LiveCalibrationWindow *ui;
    
    // Camera related
    cv::VideoCapture camera;
    QTimer *frameTimer;
    int cameraIndex;
    bool isCalibrating;
    
    // Calibration parameters
    cv::Size boardSize;
    float squareSize;
    
    // Calibration data
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;
    std::vector<cv::Mat> capturedImages;
    
    // Calibration results
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    double reprojectionError;
    
    // Helper functions
    bool findChessboardAndDraw(cv::Mat &frame, std::vector<cv::Point2f> &corners);
    bool evaluateFrameQuality(const cv::Mat &frame, const std::vector<cv::Point2f> &corners);
    void performCalibration();
    void displayCalibrationResults();
    QImage matToQImage(const cv::Mat &mat);
    void updateCameraParameters();
    std::vector<cv::Point3f> getChessboard3DPoints();
};

#endif // LIVECALIBRATIONWINDOW_H

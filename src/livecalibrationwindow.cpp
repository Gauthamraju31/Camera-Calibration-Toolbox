#include "livecalibrationwindow.h"
#include "ui_livecalibrationwindow.h"
#include <QMessageBox>
#include <sstream>
#include <iomanip>

LiveCalibrationWindow::LiveCalibrationWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiveCalibrationWindow),
    cameraIndex(0),
    isCalibrating(false),
    boardSize(cv::Size(9, 6)),  // Default chessboard size
    squareSize(25.0f),           // Default square size in mm
    reprojectionError(0.0)
{
    ui->setupUi(this);
    
    frameTimer = new QTimer(this);
    connect(frameTimer, &QTimer::timeout, this, &LiveCalibrationWindow::updateFrame);
    
    // Connect UI signals
    connect(ui->startButton, &QPushButton::clicked, this, &LiveCalibrationWindow::onStartCamera);
    connect(ui->stopButton, &QPushButton::clicked, this, &LiveCalibrationWindow::onStopCamera);
    connect(ui->captureButton, &QPushButton::clicked, this, &LiveCalibrationWindow::onCaptureFrame);
    connect(ui->resetButton, &QPushButton::clicked, this, &LiveCalibrationWindow::onResetCalibration);
    
    ui->stopButton->setEnabled(false);
    ui->captureButton->setEnabled(false);
}

LiveCalibrationWindow::~LiveCalibrationWindow()
{
    if (camera.isOpened()) {
        camera.release();
    }
    delete frameTimer;
    delete ui;
}

void LiveCalibrationWindow::onStartCamera()
{
    updateCameraParameters();
    
    camera.open(cameraIndex);
    if (!camera.isOpened()) {
        QMessageBox::critical(this, "Error", "Failed to open camera!");
        return;
    }
    
    // Set camera resolution if specified
    int width = ui->widthSpinBox->value();
    int height = ui->heightSpinBox->value();
    if (width > 0 && height > 0) {
        camera.set(cv::CAP_PROP_FRAME_WIDTH, width);
        camera.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    }
    
    isCalibrating = true;
    frameTimer->start(33); // ~30 fps
    
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->captureButton->setEnabled(true);
    ui->statusLabel->setText("Status: Camera running - Move chessboard to different positions");
}

void LiveCalibrationWindow::onStopCamera()
{
    frameTimer->stop();
    if (camera.isOpened()) {
        camera.release();
    }
    
    isCalibrating = false;
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->captureButton->setEnabled(false);
    ui->statusLabel->setText("Status: Camera stopped");
}

void LiveCalibrationWindow::updateFrame()
{
    if (!camera.isOpened()) return;
    
    cv::Mat frame;
    camera >> frame;
    
    if (frame.empty()) return;
    
    std::vector<cv::Point2f> corners;
    bool patternFound = findChessboardAndDraw(frame, corners);
    
    // Automatic sample collection - capture good frames automatically
    if (patternFound && isCalibrating) {
        bool isGoodFrame = evaluateFrameQuality(frame, corners);
        
        if (isGoodFrame) {
            ui->qualityLabel->setText("Frame Quality: GOOD - Auto-capturing...");
            ui->qualityLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
            
            // Auto-capture good frames with some delay to avoid duplicates
            static int framesSinceLastCapture = 0;
            framesSinceLastCapture++;
            
            if (framesSinceLastCapture > 30) { // Capture at most once per second
                onCaptureFrame();
                framesSinceLastCapture = 0;
            }
        } else {
            ui->qualityLabel->setText("Frame Quality: Fair - Move board to better position");
            ui->qualityLabel->setStyleSheet("QLabel { color: orange; }");
        }
    } else if (patternFound) {
        ui->qualityLabel->setText("Frame Quality: Pattern detected");
        ui->qualityLabel->setStyleSheet("QLabel { color: blue; }");
    } else {
        ui->qualityLabel->setText("Frame Quality: No pattern detected");
        ui->qualityLabel->setStyleSheet("QLabel { color: red; }");
    }
    
    // Display frame
    QImage qImg = matToQImage(frame);
    ui->videoLabel->setPixmap(QPixmap::fromImage(qImg).scaled(
        ui->videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

bool LiveCalibrationWindow::findChessboardAndDraw(cv::Mat &frame, std::vector<cv::Point2f> &corners)
{
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    
    bool found = cv::findChessboardCorners(gray, boardSize, corners,
        cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK);
    
    if (found) {
        cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
            cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
        cv::drawChessboardCorners(frame, boardSize, corners, found);
    }
    
    return found;
}

bool LiveCalibrationWindow::evaluateFrameQuality(const cv::Mat &frame, const std::vector<cv::Point2f> &corners)
{
    if (corners.empty()) return false;
    
    // Check 1: Board should not be too close or too far (area-based)
    cv::Rect boundingRect = cv::boundingRect(corners);
    double boardArea = boundingRect.area();
    double frameArea = frame.cols * frame.rows;
    double areaRatio = boardArea / frameArea;
    
    if (areaRatio < 0.15 || areaRatio > 0.65) return false;
    
    // Check 2: Board should not be at extreme angles (aspect ratio check)
    double aspectRatio = (double)boundingRect.width / boundingRect.height;
    if (aspectRatio < 0.5 || aspectRatio > 2.0) return false;
    
    // Check 3: Check blur (using Laplacian variance)
    cv::Mat gray, laplacian;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::Laplacian(gray, laplacian, CV_64F);
    cv::Scalar mean, stddev;
    cv::meanStdDev(laplacian, mean, stddev);
    double variance = stddev.val[0] * stddev.val[0];
    
    if (variance < 100) return false; // Image is too blurry
    
    // Check 4: Ensure diversity - don't capture too similar frames
    if (!capturedImages.empty()) {
        // Simple check: ensure board is in different position
        cv::Point2f center = cv::Point2f(boundingRect.x + boundingRect.width/2.0,
                                         boundingRect.y + boundingRect.height/2.0);
        
        // Check against last captured frame position
        if (imagePoints.size() > 0) {
            std::vector<cv::Point2f> lastCorners = imagePoints.back();
            cv::Rect lastRect = cv::boundingRect(lastCorners);
            cv::Point2f lastCenter = cv::Point2f(lastRect.x + lastRect.width/2.0,
                                                  lastRect.y + lastRect.height/2.0);
            
            double distance = cv::norm(center - lastCenter);
            if (distance < 50) return false; // Too close to last capture
        }
    }
    
    return true;
}

void LiveCalibrationWindow::onCaptureFrame()
{
    if (!camera.isOpened()) return;
    
    cv::Mat frame;
    camera >> frame;
    
    if (frame.empty()) return;
    
    std::vector<cv::Point2f> corners;
    bool found = findChessboardAndDraw(frame, corners);
    
    if (found) {
        imagePoints.push_back(corners);
        objectPoints.push_back(getChessboard3DPoints());
        capturedImages.push_back(frame.clone());
        
        ui->samplesLabel->setText(QString("Samples: %1").arg(imagePoints.size()));
        ui->statusLabel->setText(QString("Status: Captured %1 samples").arg(imagePoints.size()));
        
        // Perform calibration if we have enough samples
        if (imagePoints.size() >= 10) {
            performCalibration();
        }
    } else {
        ui->statusLabel->setText("Status: No chessboard pattern found!");
    }
}

void LiveCalibrationWindow::onResetCalibration()
{
    imagePoints.clear();
    objectPoints.clear();
    capturedImages.clear();
    
    cameraMatrix = cv::Mat();
    distCoeffs = cv::Mat();
    rvecs.clear();
    tvecs.clear();
    reprojectionError = 0.0;
    
    ui->samplesLabel->setText("Samples: 0");
    ui->resultsTextEdit->clear();
    ui->statusLabel->setText("Status: Calibration data reset");
}

void LiveCalibrationWindow::performCalibration()
{
    if (imagePoints.size() < 10) {
        ui->statusLabel->setText("Status: Need at least 10 samples for calibration");
        return;
    }
    
    cv::Mat frame = capturedImages[0];
    cv::Size imageSize = frame.size();
    
    cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
    
    reprojectionError = cv::calibrateCamera(
        objectPoints, imagePoints, imageSize,
        cameraMatrix, distCoeffs, rvecs, tvecs,
        cv::CALIB_FIX_K4 | cv::CALIB_FIX_K5);
    
    displayCalibrationResults();
}

void LiveCalibrationWindow::displayCalibrationResults()
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    
    ss << "=== CALIBRATION RESULTS ===\n\n";
    ss << "Number of samples: " << imagePoints.size() << "\n\n";
    
    ss << "Camera Matrix (Intrinsic Parameters):\n";
    ss << "fx: " << cameraMatrix.at<double>(0, 0) << "\n";
    ss << "fy: " << cameraMatrix.at<double>(1, 1) << "\n";
    ss << "cx: " << cameraMatrix.at<double>(0, 2) << "\n";
    ss << "cy: " << cameraMatrix.at<double>(1, 2) << "\n\n";
    
    ss << "Distortion Coefficients:\n";
    ss << "k1: " << distCoeffs.at<double>(0) << "\n";
    ss << "k2: " << distCoeffs.at<double>(1) << "\n";
    ss << "p1: " << distCoeffs.at<double>(2) << "\n";
    ss << "p2: " << distCoeffs.at<double>(3) << "\n";
    ss << "k3: " << distCoeffs.at<double>(4) << "\n\n";
    
    ss << "Reprojection Error: " << reprojectionError << " pixels\n\n";
    
    if (reprojectionError < 0.5) {
        ss << "Quality: EXCELLENT\n";
    } else if (reprojectionError < 1.0) {
        ss << "Quality: GOOD\n";
    } else if (reprojectionError < 2.0) {
        ss << "Quality: FAIR\n";
    } else {
        ss << "Quality: POOR - Consider recalibrating\n";
    }
    
    ui->resultsTextEdit->setText(QString::fromStdString(ss.str()));
    ui->statusLabel->setText(QString("Status: Calibration complete! Error: %1 pixels")
                            .arg(reprojectionError, 0, 'f', 4));
}

QImage LiveCalibrationWindow::matToQImage(const cv::Mat &mat)
{
    if (mat.empty()) return QImage();
    
    switch (mat.type()) {
        case CV_8UC4: {
            return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32).copy();
        }
        case CV_8UC3: {
            return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped().copy();
        }
        case CV_8UC1: {
            return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
        }
        default:
            break;
    }
    
    return QImage();
}

void LiveCalibrationWindow::updateCameraParameters()
{
    cameraIndex = ui->cameraIndexSpinBox->value();
    
    int rows = ui->boardRowsSpinBox->value();
    int cols = ui->boardColsSpinBox->value();
    boardSize = cv::Size(cols, rows);
    
    squareSize = ui->squareSizeSpinBox->value();
}

std::vector<cv::Point3f> LiveCalibrationWindow::getChessboard3DPoints()
{
    std::vector<cv::Point3f> points;
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            points.push_back(cv::Point3f(j * squareSize, i * squareSize, 0));
        }
    }
    return points;
}

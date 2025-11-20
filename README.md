# Camera Calibration Toolbox

A Qt-based desktop application for live camera calibration using OpenCV. This tool provides a user-friendly interface for calibrating cameras in real-time with automatic sample selection and quality assessment.

## Features

- **Live Camera Feed**: Real-time video display with adjustable resolution and camera selection
- **Automatic Sample Detection**: Automatically detects and evaluates chessboard patterns in the video feed
- **Smart Sample Collection**: Intelligently selects the best calibration samples based on:
  - Board position and size (ensures diversity in viewpoints)
  - Image sharpness (Laplacian variance-based blur detection)
  - Board angle and perspective (aspect ratio checks)
  - Position diversity (avoids capturing similar frames)
- **Real-time Calibration**: Performs calibration as samples are collected (minimum 10 samples)
- **Quality Assessment**: Visual feedback on frame quality and calibration accuracy
- **Calibration Results Display**: Shows:
  - Camera intrinsic matrix (fx, fy, cx, cy)
  - Distortion coefficients (k1, k2, p1, p2, k3)
  - Reprojection error in pixels
  - Quality rating (Excellent/Good/Fair/Poor)

## Requirements

### Dependencies
- Qt5 (5.15 or later)
  - QtCore
  - QtGui
  - QtWidgets
- OpenCV 4.x
  - Core
  - ImgProc
  - VideoIO
  - Calib3D
  - HighGUI
  - ImgCodecs
- C++11 compatible compiler

### Calibration Pattern
- Standard chessboard pattern (default: 9x6 inner corners)
- Printable chessboard patterns available at: https://docs.opencv.org/master/pattern.png
- Custom sizes supported through UI settings

## Installation

### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install qtbase5-dev qt5-qmake libopencv-dev

# Clone the repository
git clone https://github.com/Gauthamraju31/Camera-Calibration-Toolbox.git
cd Camera-Calibration-Toolbox

# Build
qmake CameraCalibration.pro
make

# Run
./CameraCalibration
```

### Other Linux Distributions
Install Qt5 development packages and OpenCV development packages using your distribution's package manager, then follow the build steps above.

## Usage

### Starting the Application
1. Launch the application: `./CameraCalibration`
2. Select "Single Camera Calibration" from the main menu
3. Click "Next" to open the live calibration window

### Calibration Process

#### 1. Configure Camera Settings
- **Camera Index**: Select the camera device (0 for default camera)
- **Resolution**: Set width and height (default: 640x480)

#### 2. Configure Calibration Pattern
- **Board Columns**: Number of inner corners horizontally (default: 9)
- **Board Rows**: Number of inner corners vertically (default: 6)
- **Square Size**: Physical size of chessboard squares in millimeters (default: 25mm)

#### 3. Start Calibration
1. Click "Start Camera" to begin the live feed
2. Hold your printed chessboard pattern in front of the camera
3. The application will automatically detect the pattern and draw corners in real-time
4. **Frame Quality Indicator** shows:
   - **GOOD (Green)**: Frame will be auto-captured
   - **Fair (Orange)**: Pattern detected but quality could be better
   - **No pattern (Red)**: Move board into view

#### 4. Collect Samples
The application uses **automatic sample collection** with intelligent quality assessment:
- Move the chessboard to different positions and angles
- The system automatically captures high-quality frames
- Aim for variety: different distances, angles, and positions across the image
- Minimum 10 samples required (15-25 samples recommended for best results)
- "Manual Capture" button available for manual control if needed

#### 5. View Results
- Calibration runs automatically after collecting 10+ samples
- Results panel shows:
  - Camera matrix (intrinsic parameters)
  - Distortion coefficients
  - Reprojection error
  - Quality assessment
- Lower reprojection error = better calibration (<0.5 pixels is excellent)

#### 6. Reset or Recalibrate
- Use "Reset Calibration" to clear all samples and start over
- Use "Stop Camera" to pause the feed

## Tips for Best Results

1. **Print Quality**: Use a high-quality print of the chessboard pattern on a flat, rigid surface
2. **Lighting**: Ensure good, even lighting to avoid shadows
3. **Variety**: Capture the board from many different angles and positions
4. **Coverage**: Move the board to all areas of the image (corners, edges, center)
5. **Distance**: Vary the distance from near to far
6. **Stability**: Hold the board steady when the system is capturing
7. **Sample Count**: Use 15-25 samples for optimal results

## Project Structure

```
Camera-Calibration-Toolbox/
├── src/
│   ├── main.cpp                      # Application entry point
│   ├── cameracalibrator.cpp          # Main window implementation
│   └── livecalibrationwindow.cpp     # Live calibration window implementation
├── inc/
│   ├── cameracalibrator.h            # Main window header
│   └── livecalibrationwindow.h       # Live calibration window header
├── forms/
│   ├── cameracalibrator.ui           # Main window UI definition
│   ├── livecalibrationwindow.ui      # Live calibration UI definition
│   └── test_window.ui                # Test window UI
├── res/
│   └── logo.jpg                      # Application logo
├── CameraCalibration.pro             # Qt project file
├── resource.qrc                      # Qt resource file
└── README.md                         # This file
```

## Algorithm Details

### Automatic Sample Selection
The application evaluates each frame based on multiple criteria:

1. **Board Size Check**: Ensures the board occupies 15-65% of the image area
2. **Perspective Check**: Validates aspect ratio (0.5-2.0) to avoid extreme angles
3. **Sharpness Check**: Uses Laplacian variance to detect and reject blurry images
4. **Diversity Check**: Ensures new samples are sufficiently different from previous ones

### Calibration Method
Uses OpenCV's `calibrateCamera` function with:
- Iterative optimization for intrinsic and extrinsic parameters
- Reprojection error minimization
- Fixed k4 and k5 distortion coefficients (CALIB_FIX_K4 | CALIB_FIX_K5)

## Troubleshooting

**Camera not detected:**
- Check camera permissions
- Verify camera is not in use by another application
- Try different camera indices (0, 1, 2, etc.)

**Pattern not detected:**
- Ensure chessboard settings match your printed pattern
- Check lighting conditions
- Verify pattern is clearly visible and not occluded
- Make sure the pattern is flat

**High reprojection error:**
- Collect more samples with better variety
- Ensure calibration pattern is flat and accurately printed
- Check for motion blur in captured samples
- Reset and recalibrate

## Future Enhancements

- Stereo camera calibration support
- Save/load calibration results
- Export calibration parameters to various formats
- Support for other calibration patterns (ArUco, circular grid)
- Undistortion preview

## License

This project is under development.

## Contributors

- Gauthamraju31

## References

- OpenCV Camera Calibration: https://docs.opencv.org/master/dc/dbb/tutorial_py_calibration.html
- Qt Documentation: https://doc.qt.io/


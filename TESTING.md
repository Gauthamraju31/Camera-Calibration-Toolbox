# Testing the Camera Calibration Toolbox

## Prerequisites for Testing

1. **Hardware Requirements**:
   - A working webcam (USB camera or built-in laptop camera)
   - A printed chessboard calibration pattern

2. **Software Requirements**:
   - X11 display server (for GUI)
   - Camera permissions

## Building and Running

```bash
# Build the application
qmake CameraCalibration.pro
make

# Run the application
./CameraCalibration
```

## Quick Test Workflow

### 1. Verify Application Launch
The application should open showing the main menu with three options:
- Single Camera Calibration
- Stereo Camera Calibration
- Calibration Error Testing

### 2. Test Camera Detection
1. Click "Single Camera Calibration"
2. Click "Next"
3. The live calibration window should open
4. Click "Start Camera"
5. Verify that your camera feed appears in the video display area

### 3. Test Pattern Detection
1. Hold a chessboard pattern (9x6 default) in front of the camera
2. The application should:
   - Draw colored corners on detected patterns
   - Show "Pattern detected" in the quality label
   - Turn green when frame quality is good

### 4. Test Automatic Sample Collection
1. Move the chessboard to different positions:
   - Top-left corner of the view
   - Top-right corner
   - Bottom-left corner
   - Bottom-right corner
   - Center at different distances
2. Watch the "Samples" counter increase automatically
3. Each good sample should be captured automatically when quality is high

### 5. Test Calibration Results
1. After collecting 10+ samples, calibration runs automatically
2. Verify the results panel shows:
   - Camera Matrix values (fx, fy, cx, cy)
   - Distortion Coefficients (k1, k2, p1, p2, k3)
   - Reprojection Error
   - Quality rating

### 6. Test Controls
- **Stop Camera**: Should stop the video feed
- **Manual Capture**: Should capture current frame with detected pattern
- **Reset Calibration**: Should clear all samples and results

## Expected Output Examples

### Good Calibration Result
```
=== CALIBRATION RESULTS ===

Number of samples: 20

Camera Matrix (Intrinsic Parameters):
fx: 520.4523
fy: 521.2341
cx: 320.1145
cy: 240.8892

Distortion Coefficients:
k1: -0.2851
k2: 0.0921
p1: -0.0003
p2: 0.0001
k3: 0.0000

Reprojection Error: 0.3452 pixels

Quality: EXCELLENT
```

### Frame Quality Indicators

| Indicator | Meaning | Action Required |
|-----------|---------|----------------|
| **GOOD (Green)** | High quality frame, will be auto-captured | Continue moving board |
| **Fair (Orange)** | Pattern detected but quality could improve | Adjust position/angle |
| **No pattern (Red)** | Pattern not detected | Move board into view |

## Testing Without Physical Hardware

If you don't have a camera or chessboard pattern, you can still verify the build:

```bash
# Verify the executable is built correctly
file ./CameraCalibration
# Should output: ELF 64-bit LSB pie executable...

# Check linked libraries
ldd ./CameraCalibration | grep -E '(Qt|opencv)'
# Should show Qt5 and OpenCV libraries

# Verify UI files were processed
ls -la ui_*.h
# Should show generated UI headers
```

## Common Issues and Solutions

### Issue: "Failed to open camera"
**Solution**: 
- Check camera permissions
- Verify camera is not in use by another app
- Try different camera index (0, 1, 2, etc.)

### Issue: "No pattern detected"
**Solution**:
- Verify chessboard settings match your printed pattern
- Ensure good lighting
- Check pattern is flat and clearly visible
- Make sure all corners are in view

### Issue: GUI doesn't start
**Solution**:
- Ensure X11 is running: `echo $DISPLAY`
- Try: `export DISPLAY=:0`
- Check Qt installation: `qmake --version`

### Issue: High reprojection error (>2.0)
**Solution**:
- Collect more samples (20-25)
- Ensure better variety in positions
- Check pattern flatness
- Verify square size measurement

## Automated Build Verification

You can verify the code compiles without errors:

```bash
# Clean build test
make clean
qmake CameraCalibration.pro
make

# Check for warnings (should be minimal)
make 2>&1 | grep -i warning

# Verify executable exists and is linked correctly
test -x ./CameraCalibration && echo "Build successful!" || echo "Build failed!"
```

## Manual Code Inspection Points

When reviewing the code, check:

1. **livecalibrationwindow.cpp**:
   - `evaluateFrameQuality()`: Multi-criteria quality assessment
   - `findChessboardAndDraw()`: Real-time pattern detection
   - `performCalibration()`: OpenCV calibration algorithm

2. **livecalibrationwindow.h**:
   - Member variables for calibration data storage
   - Signal/slot connections for UI events

3. **livecalibrationwindow.ui**:
   - Layout structure with video feed and results panels
   - Control buttons and parameter inputs

## Performance Benchmarks

Expected performance on modern hardware:
- Frame processing: ~30 FPS at 640x480
- Pattern detection: <50ms per frame
- Calibration computation: <1 second for 20 samples
- Memory usage: <100MB

## Integration Testing

For integration with other applications:

```cpp
// Example: Using calibration results in C++ with OpenCV
cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << 
    520.45, 0, 320.11,
    0, 521.23, 240.89,
    0, 0, 1);

cv::Mat distCoeffs = (cv::Mat_<double>(5,1) << 
    -0.2851, 0.0921, -0.0003, 0.0001, 0.0);

// Undistort an image
cv::Mat undistorted;
cv::undistort(image, undistorted, cameraMatrix, distCoeffs);
```

## Continuous Integration

To run in CI/CD pipelines:

```bash
# Build only (no GUI execution)
qmake CameraCalibration.pro CONFIG+=headless
make

# Verify build artifacts
test -f CameraCalibration
```

## Screenshots

When testing, you should see:

1. **Main Menu**: Three large buttons for calibration modes
2. **Live Calibration Window**: 
   - Left panel with camera and pattern settings
   - Center area with live video feed
   - Right panel with calibration results
   - Status indicators at the bottom

## Reporting Test Results

When reporting issues, please include:
- OpenCV version: `pkg-config --modversion opencv4`
- Qt version: `qmake --version`
- Camera model and connection type
- Sample calibration output
- Any error messages from console

# Camera Calibration Toolbox - Quick Start Guide

## What is Camera Calibration?

Camera calibration is the process of determining the internal parameters of a camera (intrinsic parameters like focal length and optical center) and lens distortion coefficients. This is essential for:

- Computer vision applications
- 3D reconstruction
- Augmented reality
- Robot navigation
- Measurement systems

## Quick Start (5 Minutes)

### Prerequisites
1. A USB webcam or built-in laptop camera
2. A printed chessboard pattern (download from: https://docs.opencv.org/master/pattern.png)
   - Print on A4/Letter paper
   - Mount on cardboard for flatness
   - Default pattern is 9x6 inner corners

### Steps

1. **Start the Application**
   ```bash
   ./CameraCalibration
   ```

2. **Select Calibration Mode**
   - Click "Single Camera Calibration"
   - Click "Next"

3. **Configure Settings** (Optional - defaults work well)
   - Camera Index: 0 (first camera)
   - Resolution: 640x480
   - Pattern: 9x6 corners, 25mm squares

4. **Begin Calibration**
   - Click "Start Camera"
   - Hold the chessboard pattern in front of the camera

5. **Collect Samples** (Automatic)
   - Move the board slowly around the camera view
   - The system will automatically capture good frames
   - Watch the "Frame Quality" indicator:
     - **GREEN**: Good frame - being captured
     - **ORANGE**: Fair frame - improve position
     - **RED**: No pattern detected
   
6. **Position Variety** - Move the board to:
   - All four corners of the view
   - Near and far from the camera
   - Different angles (tilted left, right, up, down)
   - Center of the view
   
7. **Wait for Results**
   - Minimum 10 samples needed
   - 15-25 samples recommended
   - Calibration runs automatically
   - Results appear in the right panel

8. **Evaluate Results**
   - Reprojection Error shown in pixels
   - Lower is better:
     - < 0.5: Excellent
     - 0.5-1.0: Good
     - 1.0-2.0: Fair
     - > 2.0: Poor (recalibrate)

## Sample Capture Strategy

For best results, capture the board in these positions:

```
1. Top-Left      2. Top-Center    3. Top-Right
   ┌─────┐          ┌─────┐           ┌─────┐
   │ ▓▓  │          │ ▓▓  │           │  ▓▓ │
   │     │          │     │           │     │
   └─────┘          └─────┘           └─────┘

4. Center-Left   5. Center         6. Center-Right
   ┌─────┐          ┌─────┐           ┌─────┐
   │     │          │     │           │     │
   │ ▓▓  │          │ ▓▓  │           │  ▓▓ │
   └─────┘          └─────┘           └─────┘

7. Bottom-Left   8. Bottom-Center  9. Bottom-Right
   ┌─────┐          ┌─────┐           ┌─────┐
   │     │          │     │           │     │
   │ ▓▓  │          │ ▓▓  │           │  ▓▓ │
   └─────┘          └─────┘           └─────┘

10. Close-up     11. Far away      12. Tilted
```

## Understanding the Results

### Camera Matrix (Intrinsic Parameters)
```
fx, fy: Focal lengths in pixels
cx, cy: Optical center (principal point)
```

Example:
```
fx: 520.45
fy: 521.23
cx: 320.11
cy: 240.89
```
This camera has focal length ~520 pixels and optical center near image center (320, 240 for 640x480 image).

### Distortion Coefficients
```
k1, k2, k3: Radial distortion
p1, p2: Tangential distortion
```

Example:
```
k1: -0.2851
k2: 0.0921
p1: -0.0003
p2: 0.0001
k3: 0.0
```
Negative k1 indicates barrel distortion (common in webcams).

### Reprojection Error
The average distance (in pixels) between detected corners and reprojected corners using the calibration. Lower is better.

## Troubleshooting

### "No pattern detected"
- Ensure the chessboard is fully visible
- Check that the pattern settings match your printed board
- Improve lighting
- Reduce motion blur (hold board steady)

### "Frame Quality: Fair" (won't auto-capture)
- Move board to a different position
- Change distance from camera
- Adjust angle
- Ensure board is not at extreme angles

### High reprojection error (>2.0 pixels)
- Collect more samples (aim for 20-25)
- Ensure better variety in positions
- Check that the pattern is perfectly flat
- Verify accurate square size measurement
- Reset and try again

### Camera not starting
- Check camera is connected
- Try different camera index (1, 2, etc.)
- Close other applications using the camera
- Check camera permissions

## Advanced Usage

### Custom Chessboard Pattern

If using a different pattern:
1. Measure the actual square size with a ruler (in millimeters)
2. Count inner corners (not squares):
   - For a 10x7 squares board → 9x6 corners
3. Enter values in "Calibration Pattern" settings before starting

### Manual Capture Mode

If automatic capture is too aggressive or not working:
1. Start the camera
2. Position the board
3. Wait for pattern detection (corners drawn)
4. Click "Manual Capture" button
5. Repeat for different positions

### Optimal Sample Count

- Minimum: 10 samples (for quick testing)
- Recommended: 15-20 samples (good balance)
- Professional: 25-30 samples (best accuracy)
- Diminishing returns after 30 samples

## Next Steps

After calibration:
1. Note down the camera matrix and distortion coefficients
2. Use in your computer vision application
3. For OpenCV Python:
   ```python
   import numpy as np
   camera_matrix = np.array([[fx, 0, cx],
                             [0, fy, cy],
                             [0, 0, 1]])
   dist_coeffs = np.array([k1, k2, p1, p2, k3])
   ```

## Tips for Better Accuracy

1. **Print Quality**: Use high-quality printer, avoid photocopies
2. **Pattern Flatness**: Mount on rigid surface (cardboard, foam board)
3. **Square Size**: Measure accurately with a ruler
4. **Lighting**: Bright, even lighting without reflections
5. **Camera Focus**: Ensure camera is focused (autofocus enabled)
6. **Sample Variety**: More variety in positions = better calibration
7. **Steady Hands**: Hold board steady when system captures

## Common Applications

After calibrating your camera, you can:
- Remove lens distortion from images
- Measure real-world dimensions
- Perform 3D reconstruction
- Implement augmented reality
- Improve object detection accuracy
- Enable accurate robot vision

## Support

For issues or questions:
- Check the main README.md
- Review OpenCV documentation
- Ensure all dependencies are installed correctly

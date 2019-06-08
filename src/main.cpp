#include "cameracalibrator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CameraCalibrator w;
    w.show();

    return a.exec();
}

#ifndef CALIBRATE_H
#define CALIBRATE_H

using namespace cv;

double getCalcX(double x1, double y1, double x2, double y2, double x, double y);
double getRawX(double x1, double y1, double x2, double y2, double x, double y);
double getCalcY(double x1, double y1, double x2, double y2, double x, double y);
double getRawY(double x1, double y1, double x2, double y2, double x, double y);
Point getTiltedPoint(double x1, double y1, double x2, double y2, double x3, double y3, double x, double y);

#endif

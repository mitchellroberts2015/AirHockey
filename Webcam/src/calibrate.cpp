#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

double getRawX(double x1, double y1, double x2, double y2, double x, double y)
{
    if (y1==y2 || x1==x2)
    {
        return x;
    }
    double slope1 = (y2-y1)/(x2-x1);
    double intercept1 = -slope1 * x1 + y1;
    double slope2 = -1 / slope1;
    double intercept2 = y - slope2 * x;
    return (intercept1-intercept2)/(slope2-slope1);
}

double getCalcX(double x1, double y1, double x2, double y2, double x, double y)
{
    double raw = getRawX(x1,y1,x2,y2,x,y);
    return 1 - (x1-raw) / (x1-x2);
}

double getRawY(double x1, double y1, double x2, double y2, double x, double y)
{
    if (y1==y2 || x1==x2)
    {
        return .5-y;
    }
    double xCo = getRawX(x1, y1, x2, y2, x, y);
    double slope = (y2-y1)/(x2-x1);
    double intercept = -slope * x1 + y1;
    return slope*xCo+intercept;
}

double getCalcY(double x1, double y1, double x2, double y2, double x, double y)
{
    double raw = getRawY(x1,y1,x2,y2,x,y);
    return .5 - (y1-raw) / (y1-y2);
}

Point getTiltedPoint(double x1, double y1, double x2, double y2, double x3, double y3, double x, double y)
{
    double axisX = x2 + x * fabs(x2-x1);
    double axisY = y2 + (.5-y) * fabs(y2-y1);
    double xHeight = y3-y2;
    double xWidth = x3-x2;
    double xFinal = axisX + (.5-y) * xWidth;
    double yFinal = axisY + (.5-y) * xHeight-10;
    return Point((int)xFinal,(int)yFinal);
}

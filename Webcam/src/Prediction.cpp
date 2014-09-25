#include "Prediction.h"
#include <math.h>

Prediction::Prediction(double xa, double ya, double xb, double yb, double rad):
    x1(xa), y1(ya), x2(xb), y2(yb), r(rad)
{
    //ctor
}

int Prediction::getSlopeSign()
{
    double m = (y2-y1)/(x2-x1);
    return m/fabs(m);
}

int Prediction::getLastSlopeSign()
{
    double m = getLastSlope();
    return (int)(m/fabs(m));
}

void Prediction::addPoint(double x, double y)
{
    x2=x1;
    y2=y1;
    x1=x;
    y1=y;
}

double Prediction::getFirstBounce()
{
    double m = (y2-y1)/(x2-x1);
    double angle = atan(1/m);
    double period = fabs(2*(tan(angle)));
    double wall = ((-m/fabs(m)/2)-y1) / m + x1;
    wall = fmod(wall, period);
    return wall;
}

double Prediction::getLastBounce()
{
    double m = (y2-y1)/(x2-x1);
    double angle = atan(1/m);
    double period = fabs(2*(tan(angle)));
    double wall = getFirstBounce();
    if (wall > period/2){
        return wall - period/2;
    }
    else{
        return wall;
    }
}

double Prediction::getLastSlope()
{
    double m = (y2-y1)/(x2-x1);
    double angle = atan(1/m);
    double period = (1 / tan((M_PI/2)-angle))*2;
    double wall = (-m/fabs(m)*5-y1) / m + x1;
    wall = fmod(wall+period, period);
    if (getLastBounce()>period/2){
        return -m;
    }
    else{
        return m;
    }
}

double Prediction::getAngle(double s)
{
    double m = getLastSlope();
    double b = -m * getLastBounce() + m / fabs(m) / 2;
    double det = pow(r,2) * (pow(s,2) + pow(m,2)) - pow(b*s,2);
    //double det = r * (pow(m,2) * s + 1) - pow(b,2) * s;
    if (det < 0){
        if (b > 0)
            return 180;
        else
            return 0;
    }
    else{
        double xCo = ((pow(det,.5)) - b * m) / (pow(s,2) + pow(m,2));
        //double xCo = (pow(det,.5)) - b * m * s / (pow(m,2) * s + 1);
        double yCo = m * xCo + b;
        double radians = atan(yCo/(xCo*s));
        return (180/M_PI*radians)+90;
    }
}

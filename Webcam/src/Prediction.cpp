#include "Prediction.h"
#include <math.h>

Prediction::Prediction(double rad, double aspect):
    _r(imgSize/aspect*rad), _a(aspect)
{
    _x1=_x2=_y1=_y2=0;
}

bool Prediction::isReady()
{
    return _x1!=0 && _y1!=0;
}

void Prediction::clean()
{
    _x1=_x2=_y1=_y2=0;
}

int Prediction::sign(double i)
{
    if (i > 0)
        return 1;
    return -1;
}

void Prediction::addPoint(double x, double y)
{
    _x1=_x2;
    _y1=_y2;
    _x2=x;
    _y2=y;
}


double Prediction::getFirstBounce()
{
    double m = getFirstSlope();
    double period = 2*imgSize/fabs(m)/_a;
    double wall;
    if (m<0) {
        wall = _x1 + _y1/m;
    }
    else {
        wall = (_y1-(imgSize/_a))/m+_x1;
    }
    //wall = fmod(wall, period);
    return wall;
}

double Prediction::getLastBounce()
{
    double m = getFirstSlope();
    double period = 2*imgSize/fabs(m)/_a;
    double wall = getFirstBounce();
    if (wall < 0) {
        return wall;
    }
    wall = fmod(wall, period);
    if (wall > period/2){
        return wall;
    }
    else{
        return wall-period/2;
    }
}

double Prediction::getFirstSlope()
{
    return -(_y2-_y1)/(_x2-_x1);
}

int Prediction::getx2()
{
    return (int) _x2;
}

int Prediction::gety2()
{
    return (int) _y2;
}

double Prediction::getLastSlope()
{
    double m = getFirstSlope();
    double period = 2*imgSize/fabs(m)/_a;
    if (getFirstBounce() < 0) {
        return m;
    }
    else if (getFirstBounce() > period/2){
        return m;
    }
    else{
        return -m;
    }
}

double Prediction::getAngle()
{
    double m = getLastSlope();
    double x1= getLastBounce();
    double y1;
    if(x1<0) {
        if(sign(m)==1)
            y1 = -imgSize/2/_a;
        else
            y1 = imgSize/2/_a;
    }
    else{
        if(sign(m)==1)
            y1 = -imgSize/2/_a;
        else
            y1 = imgSize/2/_a;
    }
    double x2 = 0;
    double y2 = y1 - x1*m;
    double dx = x2 - x1;
    double dy = y2 - y1;
    double dr = pow(pow(dx,2)+pow(dy,2),.5);
    double d = x1*y2-y1*x2;
    double det = pow(_r*dr,2)-pow(d,2);
    if (det >= 0) {
        double x = (d*dy + fabs(dx)*pow(det,.5))/pow(dr,2);
        double y = (-d*dx+dy*pow(det,.5))/pow(dr,2);
        return 90-180/M_PI*atan(y/x);
    }
    else {
        if (y1-m*x1 < imgSize/2/_a/2)
            return 0;
        return 180;
    }
}

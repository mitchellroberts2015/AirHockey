/*
* File:   main.cpp
* Author: sagar
*
* Created on 10 September, 2012, 7:48 PM
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "Prediction.h"
#include "calibrate.h"
#include "videoProcessing.h"

using namespace cv;
using namespace std;

Mat filter(const Mat& src, Scalar minColor, Scalar maxColor);
Vec3i findCircle(Mat& image, Scalar minColor, Scalar maxColor);


Mat filter(const Mat& src, Scalar minColor, Scalar maxColor)
{
    assert(src.type() == CV_8UC3);

    Mat filtered;
    inRange(src, minColor, maxColor, filtered);
    return filtered;
}

Vec3i findCircle(Mat& image, Scalar minColor, Scalar maxColor)
{
    Mat tempImage = image;
    tempImage = filter(tempImage, minColor, maxColor); //filter for the right color
    Mat gaussKernel = getGaussianKernel(9,2,CV_32F);
    GaussianBlur(tempImage, tempImage, Size(9,9), 2); //blur it

    vector<Vec3f> circles;
    HoughCircles(tempImage, circles, CV_HOUGH_GRADIENT, 1, 10,
                 100, 30, 10, 500 // change the last two parameters
                                // (min_radius & max_radius) to detect larger circles
                 );
    double big = 0;
    int biggestC = 0;
    if (circles.size()==0)
    {
        Vec3i none(0, 0, 0);
        return none;
    }
    for( size_t i = 0; i < circles.size(); i++ )
    {
        if (circles[i][2] > big)
        {
            biggestC = i;
        }
    }
    return circles[biggestC];
}



int main() {
VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.
if (!stream1.isOpened()) { //check if video device has been initialised
cout << "cannot open camera";
}

Mat cameraFrame1;
stream1.read(cameraFrame1);
Mat cf21;
cvtColor(cameraFrame1, cf21, CV_BGR2HSV);
Vec3i b = findCircle(cf21, Scalar(100,0,0), Scalar(125,255,255));
Vec3i g = findCircle(cf21, Scalar(55,0,0), Scalar(100,255,255));
Vec3i r = findCircle(cameraFrame1, Scalar(0,0,120), Scalar(30,25,150));
Vec3i y = findCircle(cf21, Scalar(15,130,90), Scalar(40,255,155));
Mat cameraFrame2;
stream1.read(cameraFrame2);
Mat cf22;
cvtColor(cameraFrame2, cf22, CV_BGR2HSV);
Vec3i b1 = findCircle(cf22, Scalar(100,0,0), Scalar(125,255,255));
Vec3i g1 = findCircle(cf22, Scalar(55,0,0), Scalar(100,255,255));
Vec3i r1 = findCircle(cameraFrame2, Scalar(0,0,120), Scalar(30,25,150));
Vec3i y1 = findCircle(cf22, Scalar(15,130,90), Scalar(40,255,155));

Prediction puck(getCalcX(g[0],g[1],r[0],r[1],b[0],b[1]),getCalcY(g[0],g[1],y[0],y[1],b[0],b[1]),getCalcX(g1[0],g1[1],r1[0],r1[1],b1[0],b1[1]),getCalcY(g1[0],g1[1],y1[0],y1[1],b1[0],b1[1]),.25);


//unconditional loop
while (true) {
Mat cameraFrame;
stream1.read(cameraFrame);
Mat cf2;
cvtColor(cameraFrame, cf2, CV_BGR2HSV); //convert camera to HSV

Vec3i b = findCircle(cf2, Scalar(100,0,0), Scalar(125,255,255));
if (b[2]!=0){
    circle( cameraFrame, Point(b[0], b[1]), 2, Scalar(0,0,0), 3, CV_AA);
}
Vec3i g = findCircle(cf2, Scalar(55,0,0), Scalar(100,255,255));
if (g[2]!=0){
    circle( cameraFrame, Point(g[0], g[1]), 2, Scalar(0,0,0), 3, CV_AA);
}
Vec3i r = findCircle(cameraFrame, Scalar(0,0,120), Scalar(30,25,150));
if (r[2]!=0){
    circle( cameraFrame, Point(r[0], r[1]), 2, Scalar(0,0,0), 3, CV_AA);
}
Vec3i y = findCircle(cf2, Scalar(15,130,90), Scalar(40,255,155));
if (y[2]!=0){
    circle( cameraFrame, Point(y[0], y[1]), 2, Scalar(0,0,0), 3, CV_AA);
}
/*Vec3i o = findCircle(cf2, Scalar(5,0,0), Scalar(10,255,255));
if (o[2]!=0){
    circle( cameraFrame, Point(o[0], o[1]), 2, Scalar(0,0,0), 3, CV_AA);
}*/
puck.addPoint(getCalcX(g[0],g[1],r[0],r[1],b[0],b[1]),getCalcY(g[0],g[1],y[0],y[1],b[0],b[1]));

stringstream text;
text << "X: " << getCalcX(g[0],g[1],r[0],r[1],b[0],b[1]) << " Y: " << getCalcY(g[0],g[1],y[0],y[1],b[0],b[1]);
putText(cameraFrame, text.str(), Point(200,200), FONT_HERSHEY_COMPLEX, .7,
                Scalar(0, 0, 0), 1, CV_AA);

imshow("cam", cameraFrame); //display it
if (waitKey(5) >= 0)
break;
}
/*double y1 = getCalcY(.4,0,1.36,.48,.6,1);
double x1 = getCalcX(.4,0,0,.8,.6,1);
double y2 = getCalcY(.4,0,1.36,.48,.45,.75);
double x2 = getCalcX(.4,0,0,.8,.45,.75);
Prediction puck(x1,y1,x2,y2,.25);
cout << puck.getAngle() << endl;*/

return 0;
}

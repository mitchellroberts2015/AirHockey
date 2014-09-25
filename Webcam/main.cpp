/*
* File:   main.cpp
* Author: sagar
*
* Created on 10 September, 2012, 7:48 PM
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <time.h>
#include <math.h>
#include "Prediction.h"
#include "calibrate.h"
#include "vision.h"

using namespace cv;
using namespace std;

int main() {
    int frame = -1;
    clock_t t;
    Scalar redMin = Scalar(0,0,100);
    Scalar redMax = Scalar(30,30,255);
    Scalar blueMin = Scalar(0,0,0);
    Scalar blueMax = Scalar(255,30,25);
    Scalar yellowMin = Scalar(0,70,70);
    Scalar yellowMax = Scalar(50,255,255);
    Scalar greenMin = Scalar(35,50,30);
    Scalar greenMax = Scalar(70,255,70);
    VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.
    if (!stream1.isOpened()) { //check if video device has been initialised
    cout << "cannot open camera";
    }

    t = clock();


    Mat cameraFrame1;
    stream1.read(cameraFrame1);
    Mat cf3;
    cf3 = cameraFrame1;
    //cvtColor(cameraFrame, cf2, CV_BGR2HSV); //convert camera to HSV
    Vec3i r1 = findCircle(cf3, redMin, redMax, 5, 50);
    Vec3i g1 = findCircle(cf3, greenMin, greenMax, 5, 50);
    Vec3i b1 = findCircle(cf3, blueMin, blueMax, 5, 50);
    Vec3i y1 = findCircle(cf3, yellowMin, yellowMax, 5, 50);
    circle( cameraFrame1, Point(r1[0], r1[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame1, Point(g1[0], g1[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame1, Point(b1[0], b1[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame1, Point(y1[0], y1[1]), 2, Scalar(0,0,0), 3, CV_AA);

    imshow("cam", cameraFrame1); //display it
    if (waitKey(50) >= 0){
    }



    Mat cameraFrame2;
    stream1.read(cameraFrame2);
    Mat cf4;
    cf4 = cameraFrame2;
    //cvtColor(cameraFrame, cf2, CV_BGR2HSV); //convert camera to HSV
    Vec3i r2 = findCircle(cf4, redMin, redMax, 5, 50);
    Vec3i g2 = findCircle(cf4, greenMin, greenMax, 5, 50);
    Vec3i b2 = findCircle(cf4, blueMin, blueMax, 5, 50);
    Vec3i y2 = findCircle(cf4, yellowMin, yellowMax, 5, 50);
    circle( cameraFrame2, Point(r2[0], r2[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame2, Point(g2[0], g2[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame2, Point(b2[0], b2[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame2, Point(y2[0], y2[1]), 2, Scalar(0,0,0), 3, CV_AA);

    imshow("cam", cameraFrame2); //display it
    if (waitKey(50) >= 0){
    }

    double yp1 = getCalcY(g1[0],g1[1],y1[0],y1[1],b1[0],b1[1]);
    double xp1 = getCalcX(r1[0],r1[1],g1[0],g1[1],b1[0],b1[1]);
    double yp2 = getCalcY(g2[0],g2[1],y2[0],y2[1],b2[0],b2[1]);
    double xp2 = getCalcX(r2[0],r2[1],g2[0],g2[1],b2[0],b2[1]);
    Prediction puck(xp1,yp1,xp2,yp2,.25);
    double angle = puck.getAngle(getRatio(r2[0],r2[1],g2[0],g2[1],y2[0],y2[1]));



    //unconditional loop
    while (true) {
    frame++;
    stringstream fileText;
    fileText << "C:/Users/Mitchell/Pictures/frame" << frame << ".jpg";
    Mat cameraFrame;
    stream1.read(cameraFrame);
    Mat cf2;
    cf2 = cameraFrame;
    //cvtColor(cameraFrame, cf2, CV_BGR2HSV); //convert camera to HSV
    Vec3i r = findCircle(cf2, redMin, redMax, 3, 50);
    Vec3i g = findCircle(cf2, greenMin, greenMax, 3, 50);
    Vec3i b = findCircle(cf2, blueMin, blueMax, 3, 50);
    Vec3i y = findCircle(cf2, yellowMin, yellowMax, 3, 50);
    circle( cameraFrame, Point(r[0], r[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame, Point(g[0], g[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame, Point(b[0], b[1]), 2, Scalar(0,0,0), 3, CV_AA);
    circle( cameraFrame, Point(y[0], y[1]), 2, Scalar(0,0,0), 3, CV_AA);

    double yp = getCalcY(g[0],g[1],y[0],y[1],b[0],b[1]);
    double xp = getCalcX(r[0],r[1],g[0],g[1],b[0],b[1]);
    puck.addPoint(xp,yp);
    double angle = puck.getAngle(getRatio(r2[0],r2[1],g2[0],g2[1],y2[0],y2[1]));
    stringstream angleText;
    angleText << angle;
    t = clock() - t;
    stringstream timeText;
    timeText << float(t) / CLOCKS_PER_SEC;
    t = clock();
    putText(cameraFrame, angleText.str(), Point(200,200), FONT_HERSHEY_COMPLEX, 3,
                Scalar(0,0,0), 5, CV_AA);
    putText(cameraFrame, timeText.str(), Point(200,400), FONT_HERSHEY_COMPLEX, 3,
                Scalar(0,0,0), 5, CV_AA);
    imshow("cam", cameraFrame); //display it
    if (waitKey(50) >= 0)
        break;
    double wtf = puck.getSlopeSign();
    Point hit1 = getTiltedPoint(r[0], r[1], g[0], g[1], y[0], y[1], puck.getFirstBounce(), -wtf/2);
    Point hit2 = getTiltedPoint(r[0], r[1], g[0], g[1], y[0], y[1], puck.getLastBounce(),  -1/puck.getLastSlope()*-puck.getLastBounce()+puck.getLastSlopeSign()/2);
    Point hit3 = getTiltedPoint(r[0], r[1], g[0], g[1], y[0], y[1], 0, puck.getLastSlopeSign()/2);
    circle( cameraFrame, getTiltedPoint(r[0], r[1], g[0], g[1], y[0], y[1],0,0), 2, Scalar(0,0,255), 3, CV_AA);
    line(cameraFrame, getTiltedPoint(r[0],r[1],g[0],g[1],y[0],y[1],getCalcX(r[0],r[1],g[0],g[1],b[0],b[1]),getCalcY(g[0],g[1],y[0],y[1],b[0],b[1])), hit1, Scalar(0,255,0),3);
    line(cameraFrame, hit2, hit3, Scalar(255,0,0),3);
    imshow("cam", cameraFrame); //display it
    imwrite( fileText.str(), cameraFrame );
    if (waitKey(50) >= 0)
        break;
    }

    return 0;
}

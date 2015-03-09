/*
* File:   main.cpp
* Author: The Realest
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <time.h>
#include <math.h>
#include "Prediction.h"
#include "calibrate.h"
#include "TiltedImage.h"
#include "vision.h"
#include "Serial.h"

using namespace cv;
using namespace std;

void CallBackFunc(int event, int x, int y, int flags, void* userdata);

int numClicks = 4;
Mat threshPic;
Scalar colors2[8];

int main()
{
    int frame = -1;
    clock_t t = clock();
    int imgSize = 640;
    //corner, length, width, puck
    //green, blue, orange, pink
    //Scalar colors[] = {Scalar(23,34,155), Scalar(51,82,255), Scalar(90,93,90), Scalar(118,198,200), Scalar(2,163,70), Scalar(15,255,255), Scalar(165,59,136), Scalar(183,172,186)};
    double aspect = 2;
    VideoCapture stream1(1);   //0 is the id of video device.0 if you have only one camera.
    if (!stream1.isOpened())   //check if video device has been initialised
    {
        cout << "cannot open camera";
        return -1;
    }

    CSerial serial;
    if (serial.Open(4, 115200))
        //AfxMessageBox("Port opened successfully");
        cout << "Port opened successfully\n";
    else
    {
        //AfxMessageBox("Failed to open port!");
        cout << "Failed to open port!\n";
        return -1;
    }
    char buffer[1];
    buffer[0] = 90;

    //cvtColor(cameraFrame, cf2, CV_BGR2HSV); //convert camera to HSV
    stream1.read(threshPic);
    imshow("threshold", threshPic);
    setMouseCallback("threshold", CallBackFunc, NULL);

    while (numClicks < 4)
    {
        imshow("threshold", threshPic);
        waitKey(1);
    }
    destroyWindow("threshold");

    Scalar colors[] = {Scalar(36,70,108), Scalar(56,118,164), Scalar(103,150,101), Scalar(255,211,153), Scalar(2,221,70), Scalar(15,255,134), Scalar(131,29,123), Scalar(175,105,178)};

    Prediction prediction(.25, aspect);

    int i = 0;
    Mat cf3;
    while (!prediction.isReady())
    {
        //stringstream filename;
        //filename << "C:/Users/Mitchell/Pictures/still" << i << ".jpg";
        //i++;
        Mat cameraFrame;
        //cameraFrame = imread(filename.str());
        stream1.read(cameraFrame);
        //cameraFrame = imread("C:/Users/Mitchell/Pictures/thresh5.jpg");
        //imwrite("C:/Users/Mitchell/Pictures/thresh5.jpg", cameraFrame);
        cvtColor(cameraFrame, cameraFrame, CV_BGR2HSV); //convert camera to HSV
        TiltedImage tilt(cameraFrame, colors, aspect);
        cameraFrame = tilt.getFixed();
        if (cameraFrame.empty())
        {
            prediction.clean();
            continue;
        }
        Mat cf2;
        cvtColor(cameraFrame, cf2, CV_HSV2BGR);
        cf3=cf2;
        Vec3i foundCircle = findCircle(cameraFrame, colors[6], colors[7],7,200,101);
        if ((foundCircle[0]>0 || foundCircle[1]>0))
        {
            imshow("cf",cf2);
            waitKey(1);
            stringstream file;
            file << "C:/Users/Mitchell/Pictures/gotchas" << i << ".jpg";
            i++;
            imwrite(file.str(),cf2);
            prediction.addPoint(foundCircle[0], foundCircle[1]);
        }
        else
        {
            prediction.clean();
        }
    }

    Scalar green(0, 255, 0);
    Scalar red(0, 0, 255);

    Point2i pt1 = Point2i(prediction.getx2(), prediction.gety2());
    Point2i pt2;
    if (prediction.getFirstSlope() < 0)
    {
        pt2 = Point2i(prediction.getFirstBounce(), 0);
    }
    else
    {
        pt2 = Point2i(prediction.getFirstBounce(), imgSize/aspect);
    }

    circle(cf3, Point(0, imgSize/2/aspect), (imgSize/2/aspect*.25), red, 4, 8);
    line(cf3, pt1, pt2, green, 4, 8);

    imshow("cf",cf3);
    waitKey(1);

























    while (true)
    {
        //stringstream filename;
        //filename << "C:/Users/Mitchell/Pictures/still" << i << ".jpg";
        //i++;
        Mat cameraFrame;
        //cameraFrame = imread(filename.str());
        stream1.read(cameraFrame);
        //imwrite("C:/Users/Mitchell/Pictures/thresh5.jpg", cameraFrame);
        //waitKey(1);
        cvtColor(cameraFrame, cameraFrame, CV_BGR2HSV); //convert camera to HSV
        TiltedImage tilt(cameraFrame, colors, aspect);
        cameraFrame = tilt.getFixed();
        if (cameraFrame.empty())
        {
            cout << "Sending: 255\n";
            buffer[0] = 255;
            serial.SendData(buffer, 1);
            continue;
        }
        Mat cf2;
        cvtColor(cameraFrame, cf2, CV_HSV2BGR);
        cf3=cf2;
        Vec3i foundCircle = findCircle(cameraFrame, colors[6], colors[7],7,200,101);
        if ((foundCircle[0]>0 || foundCircle[1]>0))
        {
            //imshow("cf",cf2);
            //waitKey(1);
            //stringstream file;
            //file << "C:/Users/Mitchell/Pictures/gotchas" << i << ".jpg";
            //i++;
            //imwrite(file.str(),cf2);
            prediction.addPoint(foundCircle[0], foundCircle[1]);
            pt1 = Point2i(prediction.getx2(), prediction.gety2());

            if (prediction.getFirstSlope() < 0)
            {
                pt2 = Point2i(prediction.getFirstBounce(), 0);
            }
            else
            {
                pt2 = Point2i(prediction.getFirstBounce(), imgSize/aspect);
            }

            circle(cf3, Point(0, imgSize/2/aspect), (imgSize/2/aspect*.25), red, 4, 8);
            line(cf3, pt1, pt2, green, 4, 8);

            cout << "Sending: " << prediction.getAngle() << "\n";
            buffer[0] = (int) prediction.getAngle();
            serial.SendData(buffer, 1);

            imshow("cf",cf3);
            waitKey(1);

            buffer[0] = (int) prediction.getAngle();
            serial.SendData(buffer, 1);
        }
        else
        {
            //prediction.clean();
            cout << "Sending: 255\n";
            buffer[0] = 255;
            serial.SendData(buffer, 1);
        }
    }


























#if 0


    while (true)
    {
        //stringstream filename;
        //filename << "C:/Users/Mitchell/Pictures/still" << i << ".jpg";
        //i++;
        Mat cameraFrame;
        //cameraFrame = imread(filename.str());
        stream1.read(cameraFrame);
        cvtColor(cameraFrame, cameraFrame, CV_BGR2HSV); //convert camera to HSV
        TiltedImage tilt(cameraFrame, colors, aspect);
        cameraFrame = tilt.getFixed();
        if (cameraFrame.empty())
        {
            //prediction.clean();
            continue;
        }
        Mat cf2;
        cvtColor(cameraFrame, cf2, CV_HSV2BGR);
        cf3=cf2;
        Vec3i foundCircle = findCircle(cameraFrame, colors[6], colors[7],30,200);
        if ((foundCircle[0]>0 || foundCircle[1]>0))
        {
            imshow("cf",cf2);
            stringstream file;
            file << "C:/Users/Mitchell/Pictures/gotchas" << i << ".jpg";
            i++;
            //imwrite(file.str(),cf2);
            waitKey(1);
            prediction.addPoint(foundCircle[0], foundCircle[1]);
        }
        else
        {
            //prediction.clean();
            continue;
        }


        Point2i pt1 = Point2i(prediction.getx2(), prediction.gety2());
        Point2i pt2;
        if (prediction.getFirstSlope() < 0)
        {
            pt2 = Point2i(prediction.getFirstBounce(), 0);
        }
        else
        {
            pt2 = Point2i(prediction.getFirstBounce(), 1200/aspect);
        }

        circle(cf3, Point(0, 600/aspect), (1200/aspect*.25), red, 4, 8);
        line(cf3, pt1, pt2, green, 4, 8);

        imshow("cf",cf3);
        if (waitKey(1) >= 0)
        {
            break;
        }

    }
#endif


    //unconditional loop
    /*while (true) {
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
    double angle = puck.getAngle();
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
    if ((50) >= 0)
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
    */
    return 0;
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        Vec3b color = threshPic.at<Vec3b>(Point(x,y));
        colors2[2*numClicks] = Scalar(color[0]-10, color[1]-10, color[2]-10);
        colors2[2*numClicks+1] = Scalar(color[0]+10, color[1]+10, color[2]+10);

        cout << "Min Threshold " << numClicks << ": " << color[0] - 10 << ", " << color[1] - 10 << ", " << color[2] - 10 << endl;
        cout << "Max Threshold " << numClicks << ": " << color[0] + 10 << ", " << color[1] + 10 << ", " << color[2] + 10 << endl;

        numClicks++;
    }
}

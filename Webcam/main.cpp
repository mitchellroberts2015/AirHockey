/*
* File:   main.cpp
* Author: Mitchell Roberts
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
#include <ctime>
#include "settings.h"

using namespace cv;
using namespace std;

Mat threshPic;
Scalar colors2[2];
int hueLow, hueHigh, satLow, satHigh, valLow, valHigh;

Mat colorFilter (Mat& image, Scalar& minColor, Scalar& maxColor);
void threshold(Mat &image);

int main()
{
    int frame = -1;
    clock_t t = clock();
    int imgSize = 640;

#ifdef OUTPUT_TIMES
    clock_t realStartTime;
    clock_t startTime;
    clock_t endTime;
    double time;
#endif // OUTPUT_TIMES

    //corner, length, width, puck
    //green, blue, orange, pink
    //Scalar colors[] = {Scalar(23,34,155), Scalar(51,82,255), Scalar(90,93,90), Scalar(118,198,200), Scalar(2,163,70), Scalar(15,255,255), Scalar(165,59,136), Scalar(183,172,186)};
    double aspect = 2;
    VideoCapture stream1(CAMERA_STREAM);   //0 is the id of video device.0 if you have only one camera.
    if (!stream1.isOpened())   //check if video device has been initialised
    {
        cout << "cannot open camera";
        return -1;
    }

#ifdef ARDUINO_CONNECTED
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
#endif // ARDUINO_CONNECTED

    //cvtColor(cameraFrame, cf2, CV_BGR2HSV); //convert camera to HSV
    stream1.read(threshPic);
    threshold(threshPic);
    //Scalar colors[] = {Scalar(36,70,108), Scalar(56,118,164), Scalar(103,150,101), Scalar(255,211,153), Scalar(2,221,70), Scalar(15,255,134), Scalar(131,29,123), Scalar(175,105,178)};
    Scalar colors[] = {Scalar(hueLow, satLow, valLow), Scalar(hueHigh, satHigh, valHigh)};


    Prediction prediction(.25, aspect);

    int i = 0;
    Mat cf3;
    while (!prediction.isReady())
    {
#ifdef OUTPUT_TIMES
        realStartTime = clock();
#endif // OUTPUT_TIMES

        Mat cameraFrame;

#ifdef OUTPUT_TIMES
        startTime = clock();
#endif // OUTPUT_TIMES

        stream1.read(cameraFrame);

#ifdef FAKE_CAMERA
        cameraFrame = imread(FAKE_CAMERA);
#endif

#ifdef OUTPUT_TIMES
        endTime = clock();
        time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
        cout << "Read Image Time: " << time << endl;
#endif // OUTPUT_TIMES

        cvtColor(cameraFrame, cameraFrame, CV_BGR2HSV); //convert camera to HSV

#ifdef OUTPUT_TIMES
        startTime = clock();
#endif // OUTPUT_TIMES

        TiltedImage tilt(cameraFrame, colors, aspect);
        cameraFrame = tilt.getFixed();

#ifdef OUTPUT_TIMES
        endTime = clock();
        time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
        cout << "Complete Tilting Time: " << time << endl;
#endif // OUTPUT_TIMES


        if (cameraFrame.empty())
        {
            prediction.clean();
            continue;
        }
        Mat cf2;
        cvtColor(cameraFrame, cf2, CV_HSV2BGR);
        cf3=cf2;

#ifdef OUTPUT_TIMES
        startTime = clock();
#endif // OUTPUT_TIMES

        Vec3i foundCircle = findPuck(cameraFrame, colors[0], colors[1],5,50,9,20);

#ifdef OUTPUT_TIMES
        endTime = clock();
        time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
        cout << "Find Puck Time: " << time << endl;
#endif // OUTPUT_TIMES


        if ((foundCircle[0]>0 || foundCircle[1]>0))
        {
            imshow("cf",cf2);
            waitKey(1);
            prediction.addPoint(foundCircle[0], foundCircle[1]);
        }
        else
        {
            prediction.clean();
        }

#ifdef OUTPUT_TIMES
        endTime = clock();
        time = (endTime - realStartTime) / (double) CLOCKS_PER_SEC;
        cout << "Full Frame Time: " << time << endl << endl << endl;
#endif // OUTPUT_TIMES
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


#ifdef STOP_AFTER_FRAME
    waitKey(0);
#endif // STOP_AFTER_FRAME







    while (true)
    {

#ifdef OUTPUT_TIMES
        realStartTime = clock();
#endif // OUTPUT_TIMES

        Mat cameraFrame;

#ifdef OUTPUT_TIMES
        startTime = clock();
#endif // OUTPUT_TIMES

        stream1.read(cameraFrame);

#ifdef FAKE_CAMERA
        cameraFrame = imread(FAKE_CAMERA);
#endif

#ifdef OUTPUT_TIMES
        endTime = clock();
        time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
        cout << "Read Image Time: " << time << endl;
#endif // OUTPUT_TIMES

        cvtColor(cameraFrame, cameraFrame, CV_BGR2HSV); //convert camera to HSV

#ifdef OUTPUT_TIMES
        startTime = clock();
#endif // OUTPUT_TIMES

        TiltedImage tilt(cameraFrame, colors, aspect);
        cameraFrame = tilt.getFixed();

#ifdef OUTPUT_TIMES
        endTime = clock();
        time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
        cout << "Complete Tilting Time: " << time << endl;
#endif // OUTPUT_TIMES

        if (cameraFrame.empty())
        {
            cout << "Sending: 255\n";
#ifdef ARDUINO_CONNECTED
            buffer[0] = 255;
            serial.SendData(buffer, 1);
            continue;
#endif // ARDUINO_CONNECTED
            continue;
        }
        Mat cf2;
        cvtColor(cameraFrame, cf2, CV_HSV2BGR);
        cf3=cf2;

#ifdef OUTPUT_TIMES
        startTime = clock();
#endif // OUTPUT_TIMES

        Vec3i foundCircle = findPuck(cameraFrame, colors[0], colors[1],5,50,9,20);

#ifdef OUTPUT_TIMES
        endTime = clock();
        time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
        cout << "Find Puck Time: " << time << endl;
#endif // OUTPUT_TIMES


        if ((foundCircle[0]>0 || foundCircle[1]>0))
        {
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

            cout << "Sending: " << prediction.getAngle() << endl << endl;

#ifdef ARDUINO_CONNECTED
            buffer[0] = (int) prediction.getAngle();
            serial.SendData(buffer, 1);
#endif

#ifdef OUTPUT_TIMES
            endTime = clock();
            time = (endTime - realStartTime) / (double) CLOCKS_PER_SEC;
            cout << "Full Frame Time: " << time << endl << endl << endl;
#endif // OUTPUT_TIMES

            imshow("cf",cf3);
            waitKey(1);
#ifdef STOP_AFTER_FRAME
            waitKey(0);
#endif // STOP_AFTER_FRAME

#ifdef ARDUINO_CONNECTED
            buffer[0] = (int) prediction.getAngle();
            serial.SendData(buffer, 1);
#endif
        }
        else
        {
            //prediction.clean();
            cout << "Sending: 255\n";
#ifdef ARDUINO_CONNECTED
            buffer[0] = 255;
            serial.SendData(buffer, 1);
#endif
            continue;
        }
    }

    return 0;
}












void threshold(Mat &image)
{
    hueLow = satLow = valLow = 0;
    hueHigh = satHigh = valHigh = 255;

    Scalar minColor;
    Scalar maxColor;
    Mat filtered;

    cvtColor(image, image, CV_BGR2HSV); //convert camera to HSV

    namedWindow("Threshold", 1);
    namedWindow("Trackbars", 1);
    createTrackbar("Hue Low", "Trackbars", &hueLow, 255);
    createTrackbar("Hue High", "Trackbars", &hueHigh, 255);
    createTrackbar("Sat Low", "Trackbars", &satLow, 255);
    createTrackbar("Sat High", "Trackbars", &satHigh, 255);
    createTrackbar("Val Low", "Trackbars", &valLow, 255);
    createTrackbar("Val High", "Trackbars", &valHigh, 255);

    while (true)
    {
        minColor = Scalar(hueLow,satLow,valLow);
        maxColor = Scalar(hueHigh,satHigh,valHigh);
        filtered = colorFilter(image, minColor, maxColor);
        imshow("Threshold", filtered);
        if (waitKey(20) >=0 )
        {
            break;
        }
    }
    destroyAllWindows();
}

Mat colorFilter (Mat& image, Scalar& minColor, Scalar& maxColor)
{
    Mat bw;
    Mat temp;
    cvtColor(image, temp, CV_HSV2BGR);
    inRange(image, minColor, maxColor, bw);

    for(int y=0;y<temp.rows;y++)
    {
        for(int x=0;x<temp.cols;x++)
        {
            if (!bw.at<uchar>(Point(x,y))) {
                temp.at<Vec3b>(Point(x,y)) = Vec3b(0,0,0);
            }
        }
    }

    return temp;
}

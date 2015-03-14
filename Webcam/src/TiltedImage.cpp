#include "TiltedImage.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "vision.h"
#include "settings.h"
#include <ctime>

using namespace cv;
using namespace std;

TiltedImage::TiltedImage(Mat& tilted, Scalar (&colors)[2], double aspectTemp)
{
    aspect = aspectTemp;
    _fixed = fixImage(tilted, colors[0], colors[1], 5, 50, 9);
}

TiltedImage::~TiltedImage()
{
    //dtor
}


Mat TiltedImage::fixImage(Mat &image, Scalar minColor, Scalar maxColor, int minRad, int maxRad, int blurSize)
{
#ifdef OUTPUT_TIMES
    clock_t startTime;
    clock_t endTime;
    double time;
#endif // OUTPUT_TIMES

    Mat imgCopy = image;
    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    #endif // OUTPUT_ALL_STEPS
    Mat imgShifted;
    imgCopy = filter(imgCopy, minColor, maxColor);
    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    #endif // OUTPUT_ALL_STEPS

#ifdef BLUR
#ifdef OUTPUT_TIMES
    startTime = clock();
#endif // OUTPUT_TIMES

    GaussianBlur(imgCopy, imgCopy, Size(BLUR,BLUR),2);

#ifdef OUTPUT_TIMES
    endTime = clock();
    time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
    cout << "Blur Time: " << time << endl;
#endif // OUTPUT_TIMES
#endif
    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    #endif // OUTPUT_ALL_STEPS

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<vector<Point> > bigContours;
    vector<Point> wholeTable;
    vector<Point> convexTable;
    Point2f orderedTable[4];

#ifdef OUTPUT_TIMES
    startTime = clock();
#endif // OUTPUT_TIMES

    /// Detect edges using canny
    Canny( imgCopy, imgCopy, 150, 300, 3 );

#ifdef OUTPUT_TIMES
    endTime = clock();
    time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
    cout << "Edge Detection Time: " << time << endl;
#endif // OUTPUT_TIMES

    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    destroyWindow("edges");
    #endif // OUTPUT_ALL_STEPS

#ifdef OUTPUT_TIMES
    startTime = clock();
#endif // OUTPUT_TIMES

    /// Find contours
    findContours( imgCopy, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    for( int i = 0; i < contours.size(); i++ )
    {
        convexHull( (Mat)contours[i], contours[i], false );
    }

    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        //std::cout<<area<<std::endl;
        if (area >= M_PI * minRad * minRad && area <= M_PI * maxRad * maxRad)
        {
            bigContours.push_back(contours[i]);
        }
    }

    for (int i = 0; i < bigContours.size(); i++)
    {
        /// Get the moments
        Moments mu = moments(bigContours[i], true);

        ///  Get the mass centers:
        Point2f mc(mu.m10/mu.m00 , mu.m01/mu.m00 );

        wholeTable.push_back(mc);
    }


#ifdef OUTPUT_TIMES
    endTime = clock();
    time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
    cout << "Find All 5 Contours Time: " << time << endl;
#endif // OUTPUT_TIMES

    if (bigContours.size() != 5)
    {
        return Mat(0, 0, CV_8UC3, Scalar(0,0,255));
    }

    convexHull((Mat) wholeTable, convexTable, false);
    if (convexTable.size() != 4)
    {
        return Mat(0, 0, CV_8UC3, Scalar(0,0,255));
    }

    vector<Point2f> leftSide;
    vector<Point2f> rightSide;
    Size s = imgCopy.size();

    for (int i = 0; i < 4; i++)
    {
        //top left, bottom left, top right, bottom right
        Point point = convexTable[i];
        if (point.x > s.width / 2)
        {
            leftSide.push_back(point);
        }
        else
        {
            rightSide.push_back(point);
        }
    }

    if (leftSide[0].y < leftSide[1].y)
    {
        orderedTable[0] = (Point2f) leftSide[0];
        orderedTable[1] = (Point2f) leftSide[1];
    }
    else
    {
        orderedTable[0] = (Point2f) leftSide[1];
        orderedTable[1] = (Point2f) leftSide[0];
    }
    if (rightSide[0].y < rightSide[1].y)
    {
        orderedTable[2] = (Point2f) rightSide[0];
        orderedTable[3] = (Point2f) rightSide[1];
    }
    else
    {
        orderedTable[2] = (Point2f) rightSide[1];
        orderedTable[3] = (Point2f) rightSide[0];
    }

    Point2f rectangularTable[] = {Point2f(0,0), Point2f(0,imgSize/aspect), Point2f(imgSize,0), Point2f(imgSize,imgSize/aspect)};


#ifdef OUTPUT_TIMES
    startTime = clock();
#endif // OUTPUT_TIMES

    Mat transformation = getPerspectiveTransform(orderedTable, rectangularTable);
    warpPerspective(image, imgShifted, transformation, Size(imgSize,imgSize/aspect));

#ifdef OUTPUT_TIMES
    endTime = clock();
    time = (endTime - startTime) / (double) CLOCKS_PER_SEC;
    cout << "Warp Perspective Time: " << time << endl;
#endif // OUTPUT_TIMES

    return imgShifted;

}

Mat TiltedImage::getFixed()
{
    return _fixed;
}

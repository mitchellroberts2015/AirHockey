#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "settings.h"
using namespace cv;

Mat filter(Mat& src, Scalar minColor, Scalar maxColor)
{
    assert(src.type() == CV_8UC3);

    Mat filtered;
    inRange(src, minColor, maxColor, filtered);
    return filtered;
}

Vec3i findPuck(Mat &image, Scalar minColor, Scalar maxColor, int minRad, int maxRad, int blurSize, int border)
{
    Mat imgCopy = image;
    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    Mat imgShifted;
    #endif // OUTPUT_ALL_STEPS

    imgCopy = filter(imgCopy, minColor, maxColor);

    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    #endif // OUTPUT_ALL_STEPS

#ifdef BLUR
    GaussianBlur(imgCopy, imgCopy, Size(BLUR,BLUR), 2);
#endif // BLUR

    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    #endif // OUTPUT_ALL_STEPS

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<vector<Point> > bigContours;

    /// Detect edges using canny
    Canny( imgCopy, imgCopy, 150, 300, 3 );
    #ifdef OUTPUT_ALL_STEPS
    imshow("edges", imgCopy);
    waitKey(0);
    destroyWindow("edges");
    #endif // OUTPUT_ALL_STEPS

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

        Size s = imgCopy.size();

        if ((mc.x > border && mc.x < s.width - border) || (mc.y > border && mc.y < s.height - border))
        {
            return Vec3i(mc.x, mc.y, (int)sqrt(contourArea(bigContours[i])/M_PI));
        }
    }
    return Vec3i(0,0,0);
}

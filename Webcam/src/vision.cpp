#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;

Mat filter(Mat& src, Scalar minColor, Scalar maxColor)
{
    assert(src.type() == CV_8UC3);

    Mat filtered;
    inRange(src, minColor, maxColor, filtered);
    return filtered;
}
#if 0
Vec3i findCircle(Mat& src, Scalar minColor, Scalar maxColor, int minRad, int maxRad, int blur)
{
    Mat srcCopy = src;
    srcCopy = filter(srcCopy, minColor, maxColor); //filter for the right color
    Mat gaussKernel = getGaussianKernel(9,2,CV_32F);
    GaussianBlur(srcCopy, srcCopy, Size(blur,blur), 2); //blur it

    vector<Vec3f> circles;
    HoughCircles(srcCopy, circles, CV_HOUGH_GRADIENT, 1, 10,
                 100, 30, minRad, maxRad // change the last two parameters
                 // (min_radius & max_radius) to detect larger circles
                );
    double big = 0;
    int biggestC = 0;
    for( size_t i = 0; i < circles.size(); i++ )
    {
        if (circles[i][2] > big)
        {
            biggestC = i;
        }
    }

    if (circles.size()>0)
    {
        putText(srcCopy, "Gotcha", Point(200,200), FONT_HERSHEY_COMPLEX, 3,
                Scalar(255,255,255), 5, CV_AA);
        imshow("stuff", srcCopy);
        waitKey(0);
        return circles[biggestC];
    }
    else
    {
        imshow("stuff", srcCopy);
        waitKey(0);
        return Vec3i(0,0,0);
    }
}

#endif // 0



Vec3i findCircle(Mat& src, Scalar minColor, Scalar maxColor, int minRad, int maxRad, int blur)
{
    Mat srcCopy = src;
    srcCopy = filter(srcCopy, minColor, maxColor); //filter for the right color
    Mat gaussKernel = getGaussianKernel(9,2,CV_32F);
    GaussianBlur(srcCopy, srcCopy, Size(blur,blur), 2); //blur it

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<Point> bestCircle;

    /// Detect edges using canny
    Canny( srcCopy, srcCopy, 150, 300, 3 );
    /// Find contours
    findContours( srcCopy, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


    if (contours.size() == 0)
    {
        return Vec3i(0,0,0);
    }

    for( int i = 0; i < contours.size(); i++ )
    {
        convexHull( Mat(contours[i]), contours[i], false );
    }

    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        //std::cout<<area<<std::endl;
        if (area >= M_PI * minRad * minRad && area <= M_PI * maxRad * maxRad)
        {
            bestCircle = contours[i];
            drawContours(srcCopy, contours, i, Scalar(255, 255, 255), CV_FILLED);
            break;
        }
        if (i == contours.size()-1)
        {
            return Vec3i(0,0,0);
        }
    }

    //imshow("vision", srcCopy);
    //waitKey(0);

    /// Get the moments
    Moments mu = moments(bestCircle, true);

    ///  Get the mass centers:
    Point2f mc(mu.m10/mu.m00 , mu.m01/mu.m00 );

    return Vec3i(mc.x, mc.y, sqrt((int)(contourArea(bestCircle)/M_PI)));
}

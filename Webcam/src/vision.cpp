#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

Mat filter(Mat& src, Scalar minColor, Scalar maxColor)
{
    assert(src.type() == CV_8UC3);

    Mat filtered;
    inRange(src, minColor, maxColor, filtered);
    return filtered;
}

Vec3i findCircle(Mat& src, Scalar minColor, Scalar maxColor, int minRad, int maxRad)
{
    Mat srcCopy = src;
    srcCopy = filter(srcCopy, minColor, maxColor); //filter for the right color
    Mat gaussKernel = getGaussianKernel(9,2,CV_32F);
    GaussianBlur(srcCopy, srcCopy, Size(9,9), 2); //blur it

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
        return circles[biggestC];
    }
    else
    {
        return Vec3i(0,0,0);
    }
}

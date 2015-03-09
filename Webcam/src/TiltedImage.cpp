#include "TiltedImage.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "vision.h"

using namespace cv;

TiltedImage::TiltedImage(Mat& tilted, Scalar (&colors)[8], double aspect)
{
    fixAffine(tilted, _fixed, colors, aspect);
}

TiltedImage::~TiltedImage()
{
    //dtor
}

void TiltedImage::fixAffine(Mat& tilted, Mat& _fixed, Scalar (&colors)[8], double aspect)
{
    aspect = 2.2;
    Vec3i corner = findCircle(tilted, colors[0], colors[1], 7, 50, 9);
    Vec3i length = findCircle(tilted, colors[2], colors[3], 7, 50, 9);
    Vec3i width = findCircle(tilted, colors[4], colors[5], 7, 50, 9);

    if( (length[0]>0 || length[1]>0) && (corner[0]>0 || corner[1]>0) && (width[0]>0 || width[1]>0) )
    {
        Point2f after[3] = {Point2f(0,0), Point2f(640, 0), Point2f(0, imgSize/aspect)};
        Point2f before[3] = {Point2i(corner[0], corner[1]), Point2i(length[0], length[1]), Point2i(width[0], width[1])};
        Mat transformMatrix = getAffineTransform(before, after);
        warpAffine(tilted, _fixed, transformMatrix, Size(imgSize,imgSize/aspect));
    }
}

Mat TiltedImage::getFixed()
{
    return _fixed;
}

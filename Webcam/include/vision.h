#ifndef VISION_H_INCLUDED
#define VISION_H_INCLUDED

using namespace cv;

Mat filter(Mat& src, Scalar minColor, Scalar maxColor);
Vec3i findPuck(Mat &image, Scalar minColor, Scalar maxColor, int minRad, int maxRad, int blurSize, int border);

#endif // VISION_H_INCLUDED

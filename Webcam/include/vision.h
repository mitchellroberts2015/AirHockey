#ifndef VISION_H_INCLUDED
#define VISION_H_INCLUDED

using namespace cv;

Mat filter(Mat& src, Scalar minColor, Scalar maxColor);
Vec3i findCircle(Mat& src, Scalar minColor, Scalar maxColor, int minRad, int maxRad);

#endif // VISION_H_INCLUDED

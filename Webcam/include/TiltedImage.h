#ifndef TILTEDIMAGE_H
#define TILTEDIMAGE_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class TiltedImage
{
    public:
        TiltedImage(Mat& tilted, Scalar (&colors)[2], double aspect);
        Mat getFixed();
        int imgSize = 640;
        virtual ~TiltedImage();
    protected:
    private:
        void fixAffine(Mat& tilted, Mat& _fixed, Scalar (&colors)[8], double aspect);
        Mat fixImage(Mat &image, Scalar minColor, Scalar maxColor, int minRad, int maxRad, int blurSize);
        Mat _fixed;
        double aspect;
};

#endif // TILTEDIMAGE_H

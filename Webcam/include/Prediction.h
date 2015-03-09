#ifndef PREDICTION_H
#define PREDICTION_H


class Prediction
{
    public:
        Prediction(double rad, double aspect);
        void addPoint(double x, double y);
        double getFirstSlope();
        double getLastSlope();
        double getFirstBounce();
        double getLastBounce();
        int getx2();
        int gety2();
        int sign(double i);
        double getAngle();
        bool isReady();
        void clean();
        int imgSize = 640;
    protected:
    private:
        double _x1, _y1, _x2, _y2, _r, _a;
};

#endif // PREDICTION_H

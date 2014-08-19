#ifndef PREDICTION_H
#define PREDICTION_H


class Prediction
{
    double x1, y1, x2, y2, r;
    public:
        Prediction(double xa, double ya, double xb, double yb, double rad);
        void addPoint(double x, double y);
        double getLastBounce();
        double getLastSlope();
        double getAngle(double s);
    protected:
    private:
};

#endif // PREDICTION_H

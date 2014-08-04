double getRawX(double x1, double y1, double x2, double y2, double x, double y)
{
    if (y1==y2)
    {
        return x;
    }
    if (x1==x2)
    {
        return y;
    }
    double slope1 = (y2-y1)/(x2-x1);
    double intercept1 = -slope1 * x1 + y1;
    double slope2 = -1 / slope1;
    double intercept2 = y - slope2 * x;
    return (intercept1-intercept2)/(slope2-slope1);
}

double getCalcX(double x1, double y1, double x2, double y2, double x, double y)
{
    double raw = getRawX(x1,y1,x2,y2,x,y);
    return (x1-raw) / (x1-x2);
}

double getRawY(double x1, double y1, double x2, double y2, double x, double y)
{
    if (x1==x2)
    {
        return y;
    }
    if (y1==y2)
    {
        return x;
    }
    double xCo = getRawX(x1, y1, x2, y2, x, y);
    double slope = (y2-y1)/(x2-x1);
    double intercept = -slope * x1 + y1;
    return slope*xCo+intercept;
}

double getCalcY(double x1, double y1, double x2, double y2, double x, double y)
{
    double raw = getRawY(x1,y1,x2,y2,x,y);
    return .5 - (y1-raw) / (y1-y2);
}

#ifndef CONTROL_POINTS_H
#define CONTROL_POINTS_H

#include <array>
#include "point.h"

class ControlPoints {
    public:
        std::array<Point, 4> p;

    public:
        ControlPoints(){};
        ControlPoints(std::array<Point, 4> p);
        std::array<ControlPoints, 2> calcBezier() const;
        Point evalBezier(float t);
        Point evalBezierNormal(float t);

        
};

#endif
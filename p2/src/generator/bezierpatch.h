#ifndef BEZIER_PATCH_H
#define BEZIER_PATCH_H

#include <array>
#include "point.h"
#include "controlpoints.h"
#include "../common/vertex.h"

class BezierPatch {
    public:
        std::array<std::array<Point, 4>, 4> ps;

    public:
        BezierPatch(){};
        BezierPatch(std::array<std::array<Point, 4>, 4> ps);

        Point evalBezier(float u, float v);
        Point evalBezierNormal(float u, float v);
        
};

#endif
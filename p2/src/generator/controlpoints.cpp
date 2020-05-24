#include "controlpoints.h"

ControlPoints::ControlPoints(std::array<Point, 4> p) : p{p} {}

Point ControlPoints::evalBezier(float t){
    return Point(
          t*t*t*p[3].x + 3*t*t*(1-t)*p[2].x + 3*t*(1-t)*(1-t)*p[1].x + (1-t)*(1-t)*(1-t)*p[0].x
        , t*t*t*p[3].y + 3*t*t*(1-t)*p[2].y + 3*t*(1-t)*(1-t)*p[1].y + (1-t)*(1-t)*(1-t)*p[0].y
        , t*t*t*p[3].z + 3*t*t*(1-t)*p[2].z + 3*t*(1-t)*(1-t)*p[1].z + (1-t)*(1-t)*(1-t)*p[0].z);
}

Point ControlPoints::evalBezierNormal(float t){
    return Point(
      (-3*t*t+6*t-3)*p[0].x + (9*t*t-12*t+3)*p[1].x + (-9*t*t+6*t)*p[2].x + (3*t*t)*p[3].x
    , (-3*t*t+6*t-3)*p[0].y + (9*t*t-12*t+3)*p[1].y + (-9*t*t+6*t)*p[2].y + (3*t*t)*p[3].y
    , (-3*t*t+6*t-3)*p[0].z + (9*t*t-12*t+3)*p[1].z + (-9*t*t+6*t)*p[2].z + (3*t*t)*p[3].z
    );
}
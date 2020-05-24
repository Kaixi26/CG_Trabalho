#include "bezierpatch.h"

BezierPatch::BezierPatch(std::array<std::array<Point, 4>, 4> ps) : ps{ps} {}

Point BezierPatch::evalBezier(float u, float v){
    return ControlPoints({
          ControlPoints({ ps[0][0] , ps[1][0] , ps[2][0] , ps[3][0] }).evalBezier(u)
        , ControlPoints({ ps[0][1] , ps[1][1] , ps[2][1] , ps[3][1] }).evalBezier(u)
        , ControlPoints({ ps[0][2] , ps[1][2] , ps[2][2] , ps[3][2] }).evalBezier(u)
        , ControlPoints({ ps[0][3] , ps[1][3] , ps[2][3] , ps[3][3] }).evalBezier(u)
    }).evalBezier(v);
}

Point BezierPatch::evalBezierNormal(float u, float v){
    Point pu = ControlPoints({
          ControlPoints({ ps[0][0] , ps[1][0] , ps[2][0] , ps[3][0] }).evalBezier(u)
        , ControlPoints({ ps[0][1] , ps[1][1] , ps[2][1] , ps[3][1] }).evalBezier(u)
        , ControlPoints({ ps[0][2] , ps[1][2] , ps[2][2] , ps[3][2] }).evalBezier(u)
        , ControlPoints({ ps[0][3] , ps[1][3] , ps[2][3] , ps[3][3] }).evalBezier(u)
    }).evalBezierNormal(v);
    Point pv = ControlPoints({
          ControlPoints({ ps[0][0] , ps[0][1] , ps[0][2] , ps[0][3] }).evalBezier(v)
        , ControlPoints({ ps[1][0] , ps[1][1] , ps[1][2] , ps[1][3] }).evalBezier(v)
        , ControlPoints({ ps[2][0] , ps[2][1] , ps[2][2] , ps[2][3] }).evalBezier(v)
        , ControlPoints({ ps[3][0] , ps[3][1] , ps[3][2] , ps[3][3] }).evalBezier(v)
    }).evalBezierNormal(u);
    Vertex vert = Vertex(pu.x, pu.y, pu.z).cross(Vertex(pv.x, pv.y, pv.z));
    return Point(-vert.x, -vert.y, -vert.z);
}
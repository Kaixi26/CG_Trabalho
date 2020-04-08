#include "point.h"
#include <stdio.h>
#include <stdlib.h>

Point::Point(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

Point Point::midPoint(Point p1, Point p2){
    return Point(
        (p1.x+p2.x)/2
        , (p1.y+p2.y)/2
        , (p1.z+p2.z)/2
    );
}


void Point::print(){
    printf("%f %f %f\n", x, y, z);
}
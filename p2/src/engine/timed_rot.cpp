#include "timed_rot.h"
#include <stdio.h>

Timed_rot::Timed_rot(Vertex v, float t){
    this->v = v;
    this->time = t;
}

float Timed_rot::calc_rot(float t){
    return (360.0f*t)/this->time;
}

void Timed_rot::print(){
    printf("(%f, %f, %f) t=%f", this->v.x, this->v.y, this->v.z, this->time);
}
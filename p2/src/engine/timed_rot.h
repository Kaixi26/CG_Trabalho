#ifndef TIMED_ROT_H
#define TIMED_ROT_H

#include "../common/vertex.h"

class Timed_rot {
    public:
        Vertex v;
        float time;

    public:
        Timed_rot(Vertex, float t);
        float calc_rot(float t);
        void print();

};

#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "vertex.h"

Vertex::Vertex(float x, float y, float z){
    this->x = x;
    this->y = y;
    this->z = z;
}
Vertex Vertex::VertexPolar(const float alpha, const float beta, const float rad){
    return Vertex(rad*cos(beta)*sin(alpha),
           rad*sin(beta),
           rad*cos(beta)*cos(alpha));
}

std::tuple<int, Vertex*> Vertex::parse_array(void* buf){
    unsigned char* tmp = (unsigned char*) buf;
    int size = *(int*) tmp;
    tmp += sizeof(int);
    Vertex* vertices = (Vertex*) calloc(size, sizeof(Vertex));
    for(int i=0; i<size; i++){
        float x = *(float*) tmp;
        tmp += sizeof(float);
        float y = *(float*) tmp;
        tmp += sizeof(float);
        float z = *(float*) tmp;
        tmp += sizeof(float);
        Vertex v(x,y,z);
        vertices[i] = v;
    }
    std::tuple<int, Vertex*> ret_tuple(size, vertices);
    return ret_tuple;
}

std::tuple<int, void*> Vertex::serialize_array(Vertex* vertices, int size){
    int ret_size = sizeof(int) + size*3*sizeof(float);
    void* ret = malloc(ret_size);
    unsigned char* tmp = (unsigned char*) ret;
    *(int*) tmp = size;
    tmp += sizeof(int);
    for(int i=0; i<size; i++){
        *(float*) tmp = vertices[i].x;
        tmp += sizeof(float);
        *(float*) tmp = vertices[i].y;
        tmp += sizeof(float);
        *(float*) tmp = vertices[i].z;
        tmp += sizeof(float);
    }
    std::tuple<int, void*> ret_tuple(ret_size, ret);
    return ret_tuple;
}

void Vertex::print(){
    printf("Vertex [ %f %f %f ]", this->x, this->y, this->z);
}

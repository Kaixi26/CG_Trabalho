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

std::tuple<Vertices_t, Vertex*> Vertex::parse_array(void* buf){
    unsigned char* tmp = (unsigned char*) buf;
    Vertices_t vert_t = *(Vertices_t*) tmp;
    tmp += sizeof(Vertices_t);
    Vertex* vertices = (Vertex*) calloc(vert_t.nvertices, sizeof(Vertex));
    for(int i=0; i<vert_t.nvertices; i++){
        float x = *(float*) tmp;
        tmp += sizeof(float);
        float y = *(float*) tmp;
        tmp += sizeof(float);
        float z = *(float*) tmp;
        tmp += sizeof(float);
        Vertex v(x,y,z);
        vertices[i] = v;
    }
    std::tuple<Vertices_t, Vertex*> ret_tuple(vert_t, vertices);
    return ret_tuple;
}

std::tuple<int, void*> Vertex::serialize_array(Vertex* vertices, Vertices_t vert_t){
    vert_t.size = sizeof(Vertices_t) + vert_t.nvertices*3*sizeof(float);
    void* ret = malloc(vert_t.size);
    unsigned char* tmp = (unsigned char*) ret;
    *(Vertices_t*) tmp = vert_t;
    tmp += sizeof(Vertices_t);
    for(int i=0; i<vert_t.nvertices; i++){
        *(float*) tmp = vertices[i].x;
        tmp += sizeof(float);
        *(float*) tmp = vertices[i].y;
        tmp += sizeof(float);
        *(float*) tmp = vertices[i].z;
        tmp += sizeof(float);
    }
    std::tuple<int, void*> ret_tuple(vert_t.size, ret);
    return ret_tuple;
}

void Vertex::print(){
    printf("Vertex [ %f %f %f ]", this->x, this->y, this->z);
}

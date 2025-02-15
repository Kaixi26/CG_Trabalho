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

std::tuple<int, float*> Vertex::convert (std::tuple<int, Vertex*> tup){
    int nvert = std::get<0>(tup);
    float* vert = (float*) malloc(3*nvert*sizeof(float));
    for(int i=0; i<nvert; i++){
        vert[i*3 + 0] = std::get<1>(tup)[i].x;
        vert[i*3 + 1] = std::get<1>(tup)[i].y;
        vert[i*3 + 2] = std::get<1>(tup)[i].z;
    }
    return std::tuple<int, float*>(nvert, vert);
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


Vertex Vertex::catmull(std::array<Vertex, 4> v, float t){
    float tt = t*t;
    float ttt = t*t*t;
    return Vertex(
          (-0.5*ttt + tt - 0.5*t)*v[0].x + (1.5*ttt - 2.5*tt + 1)*v[1].x + (-1.5*ttt + 2*tt + 0.5*t)*v[2].x + (0.5*ttt - 0.5*tt)*v[3].x
        , (-0.5*ttt + tt - 0.5*t)*v[0].y + (1.5*ttt - 2.5*tt + 1)*v[1].y + (-1.5*ttt + 2*tt + 0.5*t)*v[2].y + (0.5*ttt - 0.5*tt)*v[3].y
        , (-0.5*ttt + tt - 0.5*t)*v[0].z + (1.5*ttt - 2.5*tt + 1)*v[1].z + (-1.5*ttt + 2*tt + 0.5*t)*v[2].z + (0.5*ttt - 0.5*tt)*v[3].z);
}

Vertex Vertex::cross(Vertex vert){
    return Vertex(
        this->y*vert.z - this->z*vert.y
      , this->z*vert.x - this->x*vert.z
      , this->x*vert.y - this->y*vert.x
    );
}

void Vertex::print(){
    printf("Vertex [ %f %f %f ]", this->x, this->y, this->z);
}

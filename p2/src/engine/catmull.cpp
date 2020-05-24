#include "catmull.h"
#include <stdio.h>

void Catmull::insert(Vertex v){
    this->verts.push_back(v);
}


Vertex Catmull::getAt(float te){
    float rt = (te * this->verts.size()) / time;
    int i = (int)rt;
    rt -= (int)rt;
    return Vertex::catmull(
        std::array<Vertex, 4>{
              this->verts[i%this->verts.size()]
            , this->verts[(i+1)%this->verts.size()]
            , this->verts[(i+2)%this->verts.size()]
            , this->verts[(i+3)%this->verts.size()]
            }, rt);
}

void Catmull::print(){
    for(int i=0; i < this->verts.size(); i++)
        printf(  "(%f %f %f)"
               , this->verts[i].x
               , this->verts[i].y
               , this->verts[i].z);
}
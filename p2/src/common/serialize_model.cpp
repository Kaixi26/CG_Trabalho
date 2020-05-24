#include "serialize_model.h"
#include <stdio.h>

Serialize_model::Serialize_model(std::tuple<int, float*> verts, std::tuple<int, float*> norms, std::tuple<int, float*> texs){
    this->nvert = std::get<0>(verts);
    this->vert  = std::get<1>(verts);
    this->nnorm = std::get<0>(norms);
    this->norm  = std::get<1>(norms);
    this->ntex  = std::get<0>(texs);
    this->tex   = std::get<1>(texs);
}

Serialize_model::Serialize_model(void* arr){
    char* tmp = (char*) arr;
    int off = 0;

    memcpy(&this->nvert, tmp+off, sizeof(int));
    off += sizeof(int);
    this->vert = (float*) malloc(3*this->nvert*sizeof(float));
    memcpy(this->vert, tmp+off, 3*this->nvert*sizeof(float));
    off += 3*this->nvert*sizeof(float);

    memcpy(&this->nnorm, tmp+off, sizeof(int));
    off += sizeof(int);
    this->norm = (float*) malloc(3*this->nnorm*sizeof(float));
    memcpy(this->norm, tmp+off, 3*this->nnorm*sizeof(float));
    off += 3*this->nnorm*sizeof(float);

    memcpy(&this->ntex, tmp+off, sizeof(int));
    off += sizeof(int);
    this->tex = (float*) malloc(2*this->ntex*sizeof(float));
    memcpy(this->tex, tmp+off, 2*this->ntex*sizeof(float));
    off += 2*this->ntex*sizeof(float);
}

std::tuple<int, void*> Serialize_model::serialize(){
    int size = ((this->nvert + this->nnorm)*3 + this->ntex*2)*sizeof(float) + 3*sizeof(int);
    char* arr = (char*) malloc(size);
    int off = 0;

    memcpy(arr + off, &this->nvert, sizeof(int));
    off += sizeof(int);
    memcpy(arr + off, this->vert, 3*nvert*sizeof(float));
    off += 3*nvert*sizeof(float);

    memcpy(arr + off, &this->nnorm, sizeof(int));
    off += sizeof(int);
    memcpy(arr + off, this->norm, 3*nnorm*sizeof(float));
    off += 3*nnorm*sizeof(float);

    memcpy(arr + off, &this->ntex, sizeof(int));
    off += sizeof(int);
    memcpy(arr + off, this->tex, 2*ntex*sizeof(float));
    off += 2*ntex*sizeof(float);

    return std::tuple<int, void*>(size, (void*) arr);
}

int Serialize_model::equals(Serialize_model* sm){
    if(this->nvert != sm->nvert) return 0;
    if(this->nnorm != sm->nnorm) return 0;
    if(this->ntex  != sm->ntex)  return 0;

    for(int i=0; i<this->nvert; i++) if(this->vert[i] != sm->vert[i]) return 0;
    for(int i=0; i<this->nnorm; i++) if(this->norm[i] != sm->norm[i]) return 0;
    for(int i=0; i<this->ntex; i++)  if(this->tex[i] != sm->tex[i])   return 0;

    return 1;
}
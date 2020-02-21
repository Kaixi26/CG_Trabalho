using namespace std;

#include "primitives.h"
#include "vertex.h"
#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


Primitive::Primitive(Primitive_type type, int nr_params, float* params){
    this->type = type;
    this->nr_params = nr_params;
    this->params = (float*)calloc(nr_params, sizeof(float));
    for(int i=0; i<this->nr_params; i++)
        this->params[i] = params[i];
}

/* TODO  */
Primitive::Primitive(void* buf){
    unsigned char* tmp = (unsigned char*) buf;
    this->type = *(Primitive_type*)tmp;
    tmp+=sizeof(Primitive_type);
    this->nr_params = *(int*)tmp;
    tmp+=sizeof(int);
    this->params = (float*)calloc(nr_params, sizeof(float));
    for(int i=0; i<this->nr_params; i++){
        this->params[i] = *(float*)tmp;
        tmp+=sizeof(float);
    }
}

void* Primitive::serialize(){
    size_t size = sizeof(Primitive_type) + sizeof(int) + nr_params*sizeof(float);
    void* ret = malloc(size);
    unsigned char* tmp = (unsigned char*) ret;
    *(Primitive_type*)tmp = this->type;
    tmp += sizeof(Primitive_type);
    *(int*)tmp = this->nr_params;
    tmp += sizeof(int);
    for(int i=0; i<this->nr_params; i++){
        *(float*)tmp = this->params[i];
        tmp += sizeof(float);
    }
    return ret;
}

void Primitive::print(){
    printf("PRIMITIVE %d | ", this->type);
    printf("NR_PARAMS %d | ", this->nr_params);
    printf("PARAMETERS [ ");
    for(int i=0; i<this->nr_params; i++){
        printf("%f ", params[i]);
    }
    printf("]\n");
}

std::tuple<int, Vertex*> Primitive::get_vertices_plane(){
    std::tuple<int, Vertex*> err(0, NULL);
    if(nr_params != 1) return err;
    Vertex* tmp = (Vertex*) calloc(6, sizeof(Vertex));
    tmp[0] = Vertex(params[0]/2, 0, -params[0]/2);
    tmp[1] = Vertex(params[0]/2, 0, params[0]/2);
    tmp[2] = Vertex(-params[0]/2, 0, params[0]/2);

    tmp[3] = Vertex(params[0]/2, 0, -params[0]/2);
    tmp[4] = Vertex(-params[0]/2, 0, -params[0]/2);
    tmp[5] = Vertex(-params[0]/2, 0, params[0]/2);

    std::tuple<int, Vertex*> ret(0, NULL);
    return ret;
}

std::tuple<int,Vertex*> Primitive::get_vertices(){
    switch((int)this->type){
        case PTYPE_PLANE:
            return this->get_vertices_plane();
    }
    std::tuple<int, Vertex*> err(0, NULL);
    return err;
}

Primitive_type Primitive::type_parse(const char* str){
    if(!strcmp(str, "plane")) return PTYPE_PLANE;
    else if(!strcmp(str, "box")) return PTYPE_PLANE;
    else if(!strcmp(str, "sphere")) return PTYPE_PLANE;
    else if(!strcmp(str, "cone")) return PTYPE_PLANE;
    else return PTYPE_ERR;
}


Primitive::~Primitive(){
    free(params);
}

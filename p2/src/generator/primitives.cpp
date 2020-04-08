using namespace std;

#include <tuple>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "primitives.h"
#include "../common/vertex.h"



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


std::tuple<int, BezierPatch*> Primitive::parse_bezier(FILE* f){
    int n_patches;
	int n_points;
	char tmp[1024];
	fgets(tmp, 1024, f);
	sscanf(tmp , "%d\n", &n_patches);
	std::vector<std::array<float, 16>> ind_patches;
	ind_patches.reserve(n_patches);
	for(int i=0; i<n_patches; i++){
		std::array<float, 16> curr;
		fgets(tmp, 1024, f);
		sscanf(tmp , "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", &curr[0] , &curr[1] , &curr[2] , &curr[3] , &curr[4] , &curr[5] , &curr[6] , &curr[7] , &curr[8] , &curr[9] , &curr[10] , &curr[11] , &curr[12] , &curr[13] , &curr[14] , &curr[15] , &curr[15]);
		ind_patches[i] = curr;
	}
	fgets(tmp, 1024, f);
	sscanf(tmp , "%d", &n_points);
	std::vector<Point> points;
	points.reserve(n_points);
	for(int i=0; i<n_points; i++){
		std::array<float, 3> curr;
		fgets(tmp, 1024, f);
		sscanf(tmp , "%f, %f, %f", &curr[0] , &curr[1] , &curr[2]);
		points[i] = Point(curr[0], curr[1], curr[2]);
	}
	BezierPatch* bpatches = (BezierPatch*)calloc(n_patches, sizeof(BezierPatch));
	for(int i=0; i<n_patches; i++){
		bpatches[i] = BezierPatch({
			  std::array<Point, 4>{ points[ind_patches[i][12]], points[ind_patches[i][13]], points[ind_patches[i][14]], points[ind_patches[i][15]]}
			, std::array<Point, 4>{ points[ind_patches[i][8]], points[ind_patches[i][9]], points[ind_patches[i][10]], points[ind_patches[i][11]]}
			, std::array<Point, 4>{ points[ind_patches[i][4]], points[ind_patches[i][5]], points[ind_patches[i][6]], points[ind_patches[i][7]]}
			, std::array<Point, 4>{ points[ind_patches[i][0]], points[ind_patches[i][1]], points[ind_patches[i][2]], points[ind_patches[i][3]]}
		});
	}
	return std::tuple<int, BezierPatch*> {n_patches, bpatches};  
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
    tmp[0] = Vertex(-params[0]/2, 0, params[0]/2);
    tmp[1] = Vertex(params[0]/2, 0, params[0]/2);
    tmp[2] = Vertex(params[0]/2, 0, -params[0]/2);

    tmp[3] = Vertex(params[0]/2, 0, -params[0]/2);
    tmp[4] = Vertex(-params[0]/2, 0, -params[0]/2);
    tmp[5] = Vertex(-params[0]/2, 0, params[0]/2);

    std::tuple<int, Vertex*> ret(6, tmp);
    return ret;
}


std::tuple<int, Vertex*> Primitive::get_vertices_sphere(){
    float radius = params[0];
    float slices = params[1];
    float stacks = params[2];
	float beta_d = M_PI/stacks;
	float alpha_d = (2*M_PI)/slices;
    int total_vertices = 3*2*slices*stacks;
    Vertex* tmp = (Vertex*) calloc(total_vertices, sizeof(Vertex));
    int vi = 0;
    for(int i = 0; i<slices; i++)
        for(int j = 0; j<stacks; j++){
            if(j != 0){
                tmp[vi++] = Vertex::VertexPolar(alpha_d*i-M_PI, beta_d*j-M_PI_2, radius);
                tmp[vi++] = Vertex::VertexPolar(alpha_d*(i+1)-M_PI, beta_d*j-M_PI_2, radius);
                tmp[vi++] = Vertex::VertexPolar(alpha_d*(i+1)-M_PI, beta_d*(j+1)-M_PI_2, radius);
            }
            if(j != (stacks-1)){
                tmp[vi++] = Vertex::VertexPolar(alpha_d*i-M_PI, beta_d*j-M_PI_2, radius);
                tmp[vi++] = Vertex::VertexPolar(alpha_d*(i+1)-M_PI, beta_d*(j+1)-M_PI_2, radius);
                tmp[vi++] = Vertex::VertexPolar(alpha_d*i-M_PI, beta_d*(j+1)-M_PI_2, radius);
            }
        }
    std::tuple<int, Vertex*> ret(total_vertices, tmp);
    return ret;
}

std::tuple<int, Vertex*> Primitive::get_vertices_cone(){
    float radius = params[0];
    float height = params[1];
    float slices = params[2];
    float stacks = params[3];
    const float vertical_diff = height/stacks;
    const float alpha_diff = (2*M_PI)/slices;
    int total_vertices = (3*2*slices*stacks-3) + 3*slices;
    Vertex* tmp = (Vertex*) calloc(total_vertices, sizeof(Vertex));
    int vi = 0;
    for(int i=0; i<slices; i++){
        for(int j=0; j<stacks; j++){
            float curr_rad = ((height - j*vertical_diff)*radius) / height;
            float upper_rad = ((height - (j+1)*vertical_diff)*radius) / height;
            tmp[vi++] = Vertex(sin(alpha_diff*i)*curr_rad, j*vertical_diff, cos(alpha_diff*i)*curr_rad);
            tmp[vi++] = Vertex(sin(alpha_diff*(i+1))*curr_rad, j*vertical_diff, cos(alpha_diff*(i+1))*curr_rad);
            tmp[vi++] = Vertex(sin(alpha_diff*i)*upper_rad, (j+1)*vertical_diff, cos(alpha_diff*i)*upper_rad);

            if(j == (stacks-1)) continue;
            tmp[vi++] = Vertex(sin(alpha_diff*(i+1))*curr_rad, j*vertical_diff, cos(alpha_diff*(i+1))*curr_rad);
            tmp[vi++] = Vertex(sin(alpha_diff*(i+1))*upper_rad, (j+1)*vertical_diff, cos(alpha_diff*(i+1))*upper_rad);
            tmp[vi++] = Vertex(sin(alpha_diff*i)*upper_rad, (j+1)*vertical_diff, cos(alpha_diff*i)*upper_rad);
        }
        tmp[vi++] = Vertex(0.0f, 0.0f, 0.0f);
        tmp[vi++] = Vertex(sin(alpha_diff*(i+1))*radius, 0.0f, cos(alpha_diff*(i+1))*radius);
        tmp[vi++] = Vertex(sin(alpha_diff*i)*radius, 0.0f, cos(alpha_diff*i)*radius);
    }
    std::tuple<int, Vertex*> ret(total_vertices, tmp);
    return ret;
}

std::tuple<int, Vertex*> Primitive::get_vertices_box(){
    float x = params[0];
    float y = params[1];
    float z = params[2];
    float div = params[3];
    const float d_x = x/div;
    const float d_y = y/div;
    const float d_z = z/div;
    int total_vertices = 3*(4*3*(div*div));
    Vertex* tmp = (Vertex*) calloc(total_vertices, sizeof(Vertex));
    int vi=0;
    for(int i=0; i<div; i++)
        for(int j=0; j<div; j++){
            tmp[vi++] = Vertex(j*d_x - x/2, y/2, i*d_z - z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, y/2, i*d_z - z/2);

            tmp[vi++] = Vertex(j*d_x - x/2, y/2, i*d_z - z/2);
            tmp[vi++] = Vertex(j*d_x - x/2, y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, y/2, (i+1)*d_z - z/2);

            tmp[vi++] = Vertex(j*d_x - x/2, -y/2, i*d_z - z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, -y/2, i*d_z - z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, -y/2, (i+1)*d_z - z/2);

            tmp[vi++] = Vertex(j*d_x - x/2, -y/2, i*d_z - z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, -y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex(j*d_x - x/2, -y/2, (i+1)*d_z - z/2);
        }
    for(int j=0; j<div; j++)
        for(int k=0; k<div; k++){
            tmp[vi++] = Vertex(j*d_x - x/2, k*d_y - y/2, z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, (k+1)*d_y - y/2, z/2);
            tmp[vi++] = Vertex(j*d_x - x/2, (k+1)*d_y - y/2, z/2);
            tmp[vi++] = Vertex(j*d_x - x/2, k*d_y - y/2, z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, k*d_y - y/2, z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, (k+1)*d_y - y/2, z/2);
            tmp[vi++] = Vertex(j*d_x - x/2, k*d_y - y/2, -z/2);
            tmp[vi++] = Vertex(j*d_x - x/2, (k+1)*d_y - y/2, -z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, (k+1)*d_y - y/2, -z/2);
            tmp[vi++] = Vertex(j*d_x - x/2, k*d_y - y/2, -z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, (k+1)*d_y - y/2, -z/2);
            tmp[vi++] = Vertex((j+1)*d_x - x/2, k*d_y - y/2, -z/2);
        }
    for(int i=0; i<div; i++)
        for(int k=0; k<div; k++){
            tmp[vi++] = Vertex(-x/2, k*d_y - y/2, i*d_z - z/2);
            tmp[vi++] = Vertex(-x/2, (k+1)*d_y - y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex(-x/2, (k+1)*d_y - y/2, i*d_z - z/2);
            tmp[vi++] = Vertex(-x/2, k*d_y - y/2, i*d_z - z/2);
            tmp[vi++] = Vertex(-x/2, k*d_y - y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex(-x/2, (k+1)*d_y - y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex(x/2, k*d_y - y/2, i*d_z - z/2);
            tmp[vi++] = Vertex(x/2, (k+1)*d_y - y/2, i*d_z - z/2);
            tmp[vi++] = Vertex(x/2, (k+1)*d_y - y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex(x/2, k*d_y - y/2, i*d_z - z/2);
            tmp[vi++] = Vertex(x/2, (k+1)*d_y - y/2, (i+1)*d_z - z/2);
            tmp[vi++] = Vertex(x/2, k*d_y - y/2, (i+1)*d_z - z/2);
        }
    std::tuple<int, Vertex*> ret(total_vertices, tmp);
    return ret;
}

std::tuple<int, Vertex*> Primitive::get_vertices_bezier(BezierPatch* bpatch , int size, int divu, int divv){
    int total_vertices = 6 * size * divu * divv;
    Vertex* tmp = (Vertex*) calloc(total_vertices, sizeof(Vertex));
    int vi = 0;
    Point pt;
    for(int k=0; k < size; k++)
        for(int i=0; i<divu; i++)
            for(int j=0; j<divv; j++){
                pt = bpatch[k].evalBezier(i/(float)divu, j/(float)divv);
                tmp[vi++] = Vertex(pt.x, pt.y, pt.z);
                pt = bpatch[k].evalBezier((i+1)/(float)divu, j/(float)divv);
                tmp[vi++] = Vertex(pt.x, pt.y, pt.z);
                pt = bpatch[k].evalBezier((i+1)/(float)divu, (j+1)/(float)divv);
                tmp[vi++] = Vertex(pt.x, pt.y, pt.z);
                pt = bpatch[k].evalBezier((i+1)/(float)divu, (j+1)/(float)divv);
                tmp[vi++] = Vertex(pt.x, pt.y, pt.z);
                pt = bpatch[k].evalBezier(i/(float)divu, (j+1)/(float)divv);
                tmp[vi++] = Vertex(pt.x, pt.y, pt.z);
                pt = bpatch[k].evalBezier(i/(float)divu, j/(float)divv);
                tmp[vi++] = Vertex(pt.x, pt.y, pt.z);
            }
    std::tuple<int, Vertex*> ret(total_vertices, tmp);
    return ret;
}

std::tuple<int,Vertex*> Primitive::get_vertices(){
    switch((int)this->type){
        case PTYPE_PLANE:
            return this->get_vertices_plane();
        case PTYPE_SPHERE:
            return this->get_vertices_sphere();
        case PTYPE_CONE:
            return this->get_vertices_cone();
        case PTYPE_BOX:
            return this->get_vertices_box();
    }
    std::tuple<int, Vertex*> err(0, NULL);
    return err;
}

Primitive_type Primitive::type_parse(const char* str){

    if(!strcmp(str, "plane")) return PTYPE_PLANE;
    else if(!strcmp(str, "box")) return PTYPE_BOX;
    else if(!strcmp(str, "sphere")) return PTYPE_SPHERE;
    else if(!strcmp(str, "cone")) return PTYPE_CONE;
    else return PTYPE_ERR;
}


Primitive::~Primitive(){
    free(params);
}

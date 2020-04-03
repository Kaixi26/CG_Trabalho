#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

Matrix::Matrix(float v[4][4]){
    memcpy(this->v, v, sizeof(float)*4*4);
}

Matrix Matrix::identity(){
    float tmp[4][4];
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            tmp[j][i] = (i == j) ? 1 : 0;
    return Matrix(tmp);
}

void Matrix::add(Matrix m){
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            this->v[j][i] += m.v[j][i];
}

// m * this
void Matrix::mult(Matrix m){
    float tmp[4][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            for(int n=0; n<4; n++)
                tmp[j][i] += this->v[n][i] * m.v[j][n];
    memcpy(this->v, tmp, sizeof(float)*4*4);
}

void Matrix::translate(float x, float y, float z){
    float tmp[4][4] = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0},{x,y,z,1}};
    this->mult(Matrix(tmp));
}

void Matrix::scale(float x, float y, float z){
    float tmp[4][4] = {{x,0,0,0}, {0,y,0,0}, {0,0,z,0},{0,0,0,1}};
    this->mult(Matrix(tmp));
}

void Matrix::rotate(float theta, float x, float y, float z){
    float length = x*x + y*y + z*z;
    x = x/length;
    y = y/length;
    z = z/length;
    float c = cos(theta);
    float s = sin(theta);
    float tmp[4][4] = {
        {x*x+(1-x*x)*c, x*y*(1-c)+z*s, x*z*(1-c)-y*s, 0},
        {x*y*(1-c)-z*s, y*y+(1-y*y)*c, y*z*(1-c)+x*s, 0},
        {x*z*(1-c)+y*s, y*z*(1-c)-x*s, z*z+(1-z*z)*c, 0},
        {0 , 0, 0, 1}
    };
    this->mult(Matrix(tmp));
}

Vertex Matrix::transformVertex(Vertex vertex){
    float point[4] = {vertex.x, vertex.y, vertex.z, 1};
    float tmp[4] = {0,0,0,0};
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++){
            tmp[i] += point[j] * this->v[j][i];
        }
    return Vertex(tmp[0], tmp[1], tmp[2]);
}

void Matrix::print(){
    for(int j=0; j<4; j++){
        printf("{ ");
        for(int i=0; i<4; i++)
            printf("%0.1f ", v[j][i]);
        printf("}");
    }
}
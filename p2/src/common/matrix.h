#ifndef MATRIX_H_
#define MATRIX_H_

#include <math.h>

#include "../common/vertex.h"

class Matrix {
    public:
        float v[4][4];
    public:
        Matrix(float v[4][4]);

        static Matrix identity();

        void add(Matrix);
        void mult(Matrix);

        void translate(float x, float y, float z);
        void scale(float x, float y, float z);
        void rotate(float theta, float x, float y, float z);

        Vertex transformVertex(Vertex v);

        void print();
};

#endif
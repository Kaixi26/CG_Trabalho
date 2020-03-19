#ifndef __MODEL_RTREE_H
#define __MODEL_RTREE_H

#include "../engine/tinyxml2/tinyxml2.h"
#include "../engine/model.h"
#include "../common/matrix.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace tinyxml2;

typedef enum {
RTREE_TRANSFORMATION,
RTREE_MODEL,
RTREE_UNDEFINED,
RTREE_NULL
} model_rtree_t;

class Model_rtree {
    private:
        model_rtree_t type = RTREE_UNDEFINED;
        void* data = NULL;
        size_t size = 1;
        size_t nextchild = 0;
        Model_rtree** children = (Model_rtree**)calloc(1, sizeof(Model_rtree*));
    public:
        Model_rtree(XMLNode* node);
        void draw();
        void draw(const draw_opts draw_opts);
        void print();
    private:
        void printAux(int depth);
        void add_child(Model_rtree* child);
};

#endif
#ifndef __MODEL_H
#define __MODEL_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tuple>
#include <climits>

#include "../common/vertex.h"
#include "../engine/draw_opts.h"

class Model {
    public:
        int vertex_nr = 0;
        Vertex* vertices;
        const char* filename;
    public:
        Model(const char* filename);
        void draw();
        void draw(const draw_opts opts);
};

#endif
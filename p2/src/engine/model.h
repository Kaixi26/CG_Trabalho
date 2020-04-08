#ifndef __MODEL_H
#define __MODEL_H


#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tuple>
#include <climits>
#include <vector>
 
#include "../common/vertex.h"
#include "../engine/draw_opts.h"

class Model {
    public:
        int vertex_nr = 0;
        int buffer_num;
        float* vertices;
        const char* filename;
    public:
        Model(const char* filename);
        void draw();
        void draw(const draw_opts opts);

        /* needs glewInit() to be run before */
        static void initBuffers();
};

#endif
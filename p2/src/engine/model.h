#ifndef __MODEL_H
#define __MODEL_H


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <tuple>
#include <climits>
#include <vector>
 
#include "../common/vertex.h"
#include "../common/serialize_model.h"
#include "../engine/draw_opts.h"

class Model {
    public:
        Serialize_model* sm;
        int buffer_num_vert;
        int buffer_num_norm;
        int buffer_num_tex;
        int tex_id = 0;
        float diff[4];
        float spec[4];
        float emis[4];
        float ambi[4];
        const char* filename;
        const char* texture;
    public:
        Model(const char* filename, const char* filename_textures, float diff[4], float spec[4], float emis[4], float  ambi[4]);
        void draw();
        void draw(const draw_opts opts);

        /* needs glewInit() to be run before */
        static void initBuffers();
        static void initTextures();
};

#endif
#ifndef SERIALIZE_MODEL_H
#define SERIALIZE_MODEL_H

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "../common/vertex.h"

class Serialize_model {
    public:
        int nvert = 0;
        float* vert;
        int nnorm = 0;
        float* norm;
        int ntex = 0;
        float* tex;
    public:
        Serialize_model(std::tuple<int, float*> verts, std::tuple<int, float*> norms, std::tuple<int, float*> texs);
    /* given a serialized array */
        Serialize_model(void*);
        std::tuple<int, void*> serialize();
        int equals(Serialize_model*);
};

#endif
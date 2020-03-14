#ifndef __PRIMITIVES_H_
#define __PRIMITIVES_H_

#include "../common/vertex.h"
#include <tuple>

typedef enum {
PTYPE_ERR = 0,
PTYPE_PLANE = 1,
PTYPE_BOX = 2,
PTYPE_SPHERE = 3,
PTYPE_CONE = 4
} Primitive_type;


class Primitive {
    private:
        Primitive_type type;
        int nr_params;
        float *params;

    public:
        Primitive(Primitive_type type, int nr_params, float* params);
        Primitive(void* buf);
        ~Primitive();
        void* serialize();
        void print();
        static Primitive_type type_parse(const char* str);
    private:
        std::tuple<int,Vertex*> get_vertices_plane();
        std::tuple<int, Vertex*> get_vertices_sphere();
        std::tuple<int, Vertex*> get_vertices_cone();
        std::tuple<int, Vertex*> get_vertices_box();
    public:
        std::tuple<int,Vertex*> get_vertices();
        
};


#endif // __PRIMITIVES_H_

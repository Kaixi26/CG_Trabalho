#ifndef __PRIMITIVES_H_
#define __PRIMITIVES_H_

#include "../common/vertex.h"
#include <tuple>
#include <vector>
#include "bezierpatch.h"

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
        static std::tuple<int, BezierPatch*> parse_bezier(FILE* f);
        void print();
        static Primitive_type type_parse(const char* str);
    private:
        std::tuple<int,Vertex*>  get_vertices_plane();
        std::tuple<int, Vertex*> get_normals_plane();
        std::tuple<int, float*>  get_textures_plane();
        std::tuple<int, Vertex*> get_vertices_sphere();
        std::tuple<int, Vertex*> get_normals_sphere();
        std::tuple<int, float*>  get_tex_sphere();
        std::tuple<int, Vertex*> get_vertices_cone();
        std::tuple<int, Vertex*> get_normals_cone();
        std::tuple<int, float*>  get_textures_cone();
        std::tuple<int, Vertex*> get_vertices_box();
        std::tuple<int, Vertex*> get_normals_box();
        std::tuple<int, float*>  get_textures_box();

    public:
        static std::tuple<int, Vertex*> get_vertices_bezier(BezierPatch* bpatch, int size, int divu, int divv);
        static std::tuple<int, Vertex*> get_normals_bezier(BezierPatch* bpatch, int size, int divu, int divv);
        static std::tuple<int, float*> get_textures_bezier(BezierPatch* bpatch , int size, int divu, int divv);
        std::tuple<int,Vertex*> get_vertices();
        std::tuple<int,Vertex*> get_normals();
        std::tuple<int,float*>  get_textures();
        
};


#endif // __PRIMITIVES_H_

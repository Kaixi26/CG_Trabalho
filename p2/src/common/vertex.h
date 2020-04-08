#ifndef __VERTEX_H_
#define __VERTEX_H_

#include <tuple>

#define VERTT_TRIANGLES 0
#define VERTT_TRIANGLE_STRIPS 1

typedef struct {
    int type;
    int nvertices;
    int div;
    int size;
} Vertices_t;

class Vertex{
    public:
        float x;
        float y;
        float z;
    public:
        Vertex(){};
        Vertex(float x, float y, float z);

        static Vertex VertexPolar(const float alpha, const float beta, const float rad);
        static std::tuple<Vertices_t, Vertex*> parse_array(void* buf);
        static std::tuple<int, void*> serialize_array(Vertex* vertices, Vertices_t vert_t);
        static Vertex catmull(std::array<Vertex, 4> vert, float t);
        void print();
};


#endif 
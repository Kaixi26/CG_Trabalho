#ifndef __VERTEX_H_
#define __VERTEX_H_

#include <tuple>

class Vertex{
    public:
        float x;
        float y;
        float z;
    public:
        Vertex(float x, float y, float z);

        static Vertex VertexPolar(const float alpha, const float beta, const float rad);
        static std::tuple<int, Vertex*> parse_array(void* buf);
        static std::tuple<int, void*> serialize_array(Vertex* vertices, int size);
        void print();
};


#endif // __VERTEX_H_

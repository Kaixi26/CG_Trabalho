#include "model.h"


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#endif

struct {
    int size = 1;
    int next_free = 0;
    GLuint* buffers = NULL;
} VBOBuffers;

void Model::initBuffers(){
    VBOBuffers.buffers = (GLuint*) calloc(VBOBuffers.size, sizeof(GLuint));
    glGenBuffers(VBOBuffers.size, VBOBuffers.buffers);
}

void expandBuffers(){
    VBOBuffers.buffers = (GLuint*) realloc(VBOBuffers.buffers, VBOBuffers.size*2*sizeof(GLuint));
    glGenBuffers(VBOBuffers.size, VBOBuffers.buffers + VBOBuffers.size);
    VBOBuffers.size *= 2;
}

int bindBuffer(float* data, int size){
    if(VBOBuffers.next_free >= VBOBuffers.size)
        expandBuffers();
    glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers.buffers[VBOBuffers.next_free]);
    glBufferData( GL_ARRAY_BUFFER
                , sizeof(float) * size
                , data
                , GL_STATIC_DRAW);
    return VBOBuffers.next_free++;
}

Model::Model(const char* filename){
    this->filename = filename;
    int fd = open(filename, O_RDONLY);
    if(fd == -1){
        fprintf(stderr, "Failed opening %s.\n", this->filename);
        return;
    }
    off_t fsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    void* buf = malloc(fsize + 1);
    read(fd, buf, fsize);
    close(fd);

    std::tuple<Vertices_t, Vertex*> vertices = Vertex::parse_array(buf);
    this->vertex_nr = std::get<0>(vertices).nvertices;
    this->vertices = (float*) calloc(this->vertex_nr*3, sizeof(float));
    Vertex* tmp = std::get<1>(vertices);
    for(int i=0; i<this->vertex_nr; i++){
        this->vertices[i*3]   = tmp[i].x;
        this->vertices[i*3+1] = tmp[i].y;
        this->vertices[i*3+2] = tmp[i].z;
    }
    this->buffer_num = bindBuffer(this->vertices, this->vertex_nr*3);
}

void Model::draw(){
    this->draw((draw_opts){});
}

void Model::draw(const draw_opts opts){
    srand(0);
    glBegin(GL_TRIANGLES);
    for(int i=0; i<this->vertex_nr; i++){
        if(opts.color == DRAW_OPTS_COLOR_RAND){
            if(i%3 == 0) glColor3f((float)rand()/INT_MAX, (float)rand()/INT_MAX, (float)rand()/INT_MAX);
            glVertex3f( this->vertices[i*3]
                      , this->vertices[i*3+1]
                      , this->vertices[i*3+2]);
        }
    }
    glEnd();
    if(opts.color != DRAW_OPTS_COLOR_RAND){
        glColor3f(1,1,1);
        glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers.buffers[this->buffer_num]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, this->vertex_nr);
    }
}
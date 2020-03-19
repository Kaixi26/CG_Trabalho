#include "model.h"

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

    std::tuple<int, Vertex*> vertices = Vertex::parse_array(buf);
    this->vertex_nr = std::get<0>(vertices);
    this->vertices = std::get<1>(vertices);
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
        }
        else {
            if((i/3)%2 == 0) glColor3f(1, 1, 1);
            else if((i/3)%2 == 1) glColor3f(0.5,0.5,0.5);
        }
        glVertex3f(this->vertices[i].x, this->vertices[i].y, this->vertices[i].z);
    }
    glEnd();
}
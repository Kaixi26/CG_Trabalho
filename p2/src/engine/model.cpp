#include "model.h"


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#endif

#include <IL/il.h>

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

int loadTexture(const char* str) {

	unsigned int t,tw,th;
	unsigned char *texData;
	unsigned int texID;

    ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilGenImages(1,&t);
	ilBindImage(t);
	ilLoadImage(str);
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	texData = ilGetData();

	glGenTextures(1,&texID);
	
	glBindTexture(GL_TEXTURE_2D,texID);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,		GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,		GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,   	GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

Model::Model(const char* filename, const char* filename_textures, float diff[4], float spec[4], float emis[4], float  ambi[4]){
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

    this->sm = new Serialize_model(buf);
    this->buffer_num_vert = bindBuffer(this->sm->vert, this->sm->nvert*3);
    this->buffer_num_norm = bindBuffer(this->sm->norm, this->sm->nnorm*3);
    this->buffer_num_tex  = bindBuffer(this->sm->tex,  this->sm->ntex*2);
    if(filename_textures) this->tex_id = loadTexture(filename_textures);
    memcpy(this->diff, diff, 4*sizeof(float));
    memcpy(this->spec, spec, 4*sizeof(float));
    memcpy(this->emis, emis, 4*sizeof(float));
    memcpy(this->ambi, ambi, 4*sizeof(float));
}

void Model::draw(){
    this->draw((draw_opts){});
}

void Model::draw(const draw_opts opts){
    srand(0);
    if (opts.color == DRAW_OPTS_COLOR_DEFAULT) {
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < this->sm->nvert; i++) {
            if (i % 3 == 0) glColor3f((float)rand() / INT_MAX, (float)rand() / INT_MAX, (float)rand() / INT_MAX);
            glVertex3f(this->sm->vert[i * 3], this->sm->vert[i * 3 + 1], this->sm->vert[i * 3 + 2]);
            glNormal3f(this->sm->norm[i * 3], this->sm->norm[i * 3 + 1], this->sm->norm[i * 3 + 2]);
        }
        glEnd();
    }
    if(opts.color == DRAW_OPTS_COLOR_VBO){
        glMaterialfv(GL_FRONT, GL_DIFFUSE, this->diff);
        glMaterialfv(GL_FRONT, GL_SPECULAR, this->spec);
        glMaterialfv(GL_FRONT, GL_EMISSION, this->emis);
        glMaterialfv(GL_FRONT, GL_AMBIENT, this->ambi);

        glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers.buffers[this->buffer_num_vert]);
        glVertexPointer(3,GL_FLOAT,0,0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers.buffers[this->buffer_num_norm]);
	    glNormalPointer(GL_FLOAT,0,0);

        glBindBuffer(GL_ARRAY_BUFFER, VBOBuffers.buffers[this->buffer_num_tex]);
	    glTexCoordPointer(2, GL_FLOAT,0,0);

        glBindTexture(GL_TEXTURE_2D, this->tex_id);

        glDrawArrays(GL_TRIANGLES, 0, this->sm->nvert);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "primitives.h"
#include "vertex.h"

Primitive* argparse_plane(int argc, char* argv[]){
    if(argc != 4){
        printf("WRONG NUMBER OF ARGUMENTS FOR PLANE.\n");
        printf("Usage %s plane <side_lenght> <filename>\n", argv[0]);
        return NULL;
    }
    float edge = atof(argv[2]);
    if(edge <= 0){
        printf("INVALID EDGE VALUE, PLEASE USE A VALUE ABOVE 0.");
        return NULL;
    }
    float tmp[1] = {edge};
    return new Primitive(PTYPE_PLANE, 1, tmp);
}

Primitive* argparse_sphere(int argc, char* argv[]){
    if(argc != 6){
        printf("WRONG NUMBER OF ARGUMENTS FOR SPHERE.\n");
        printf("Usage %s sphere <radius> <slices> <stacks> <filename>\n", argv[0]);
        return NULL;
    }
    float* tmp = (float*) calloc(3, sizeof(float));
    for(int i=2; i<=4; i++){
        tmp[i-2] = atof(argv[i]);
        if(tmp[i]){
            printf("All parameter values should be above 0.");
            return NULL;
        }
    }
    return new Primitive(PTYPE_SPHERE, 3, tmp);
}

Primitive* argparse_cone(int argc, char* argv[]){
    if(argc != 7){
        printf("WRONG NUMBER OF ARGUMENTS FOR CONE.\n");
        printf("Usage %s cone <radius> <height> <slices> <stacks> <filename>\n", argv[0]);
        return NULL;
    }
    float* tmp = (float*) calloc(4, sizeof(float));
    for(int i=2; i<=5; i++){
        tmp[i-2] = atof(argv[i]);
        if(tmp[i]){
            printf("All parameter values should be above 0. ('%s')", argv[i]);
            return NULL;
        }
    }
    return new Primitive(PTYPE_CONE, 4, tmp);
}

Primitive* argparse_box(int argc, char* argv[]){
    if(argc != 7){
        printf("WRONG NUMBER OF ARGUMENTS FOR BOX.\n");
        printf("Usage %s box <x dimension> <y dimension> <z dimension> <divisions> <filename>\n", argv[0]);
        return NULL;
    }
    float* tmp = (float*) calloc(4, sizeof(float));
    for(int i=2; i<=5; i++){
        tmp[i-2] = atof(argv[i]);
        if(tmp[i]){
            printf("All parameter values should be above 0. ('%s')", argv[i]);
            return NULL;
        }
    }
    return new Primitive(PTYPE_BOX, 4, tmp);
}

Primitive* argparse(int argc, char* argv[]){
    switch((int)Primitive::type_parse(argv[1])){
        case PTYPE_PLANE:
            return argparse_plane(argc, argv);
        case PTYPE_SPHERE:
            return argparse_sphere(argc, argv);
        case PTYPE_CONE:
            return argparse_cone(argc, argv);
        case PTYPE_BOX:
            return argparse_box(argc, argv);
        case PTYPE_ERR:
            printf("INVALID PRIMITIVE TYPE.\n");
            printf("POSSIBLE VALUES: 'plane' 'sphere' 'cone' 'box'\n");
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc <= 1) {
        printf("PLEASE PROVIDE PRIMITIVE ARGUMENT\n");
        printf("POSSIBLE VALUES: 'plane' 'sphere' 'cone' 'box'\n");
        return 1;
    }
    Primitive* tmp = argparse(argc, argv);
    if(tmp == NULL) return 1;

    int fd = open(argv[argc-1], O_CREAT | O_WRONLY, 0777);
    std::tuple<int,Vertex*> vertices = tmp->get_vertices();
    std::tuple<int, void*> serialized =
        Vertex::serialize_array(std::get<1>(vertices), std::get<0>(vertices));

    write(fd, std::get<1>(serialized), std::get<0>(serialized));
    delete(tmp);
    close(fd);
    printf("Done.\n");
    return 0;
}

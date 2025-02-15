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
#include <string.h>

#include "../generator/primitives.h"
#include "../common/vertex.h"
#include "../common/serialize_model.h"

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

void write_bezier(int argc, char** argv){
    FILE* fin = fopen(argv[argc-2], "r");
    int fd = open(argv[argc-1], O_CREAT | O_WRONLY, 0777);
    if(fd == -1 || fin == NULL){
        printf("Could not open file.\n");
        exit(1);
    }
    int tesselation = atoi(argv[2]);
    if(tesselation <= 0){
        printf("All parameter values should be above 0. ('%s')", argv[2]);
        exit(1);
    }
    std::tuple<int, BezierPatch*> bez = Primitive::parse_bezier(fin);
    auto vertices = Primitive::get_vertices_bezier(std::get<1>(bez), std::get<0>(bez),tesselation, tesselation);
    auto normals  = Primitive::get_normals_bezier(std::get<1>(bez), std::get<0>(bez),tesselation, tesselation);
    auto textures = Primitive::get_textures_bezier(std::get<1>(bez), std::get<0>(bez),tesselation, tesselation);
    Serialize_model* m = new Serialize_model(
        Vertex::convert(vertices)
      , Vertex::convert(normals)
      , textures);
    auto serialized = m->serialize();
    write(fd, std::get<1>(serialized), std::get<0>(serialized));

//    std::tuple<int, void*> serialized =
//        Vertex::serialize_array(std::get<1>(vertices), (Vertices_t){
//                  .type = VERTT_TRIANGLES
//                , .nvertices = std::get<0>(vertices)});
//    write(fd, std::get<1>(serialized), std::get<0>(serialized));

    printf("Done.\n");
    exit(0);
}

int main(int argc, char** argv) {
    if(argc <= 1) {
        printf("PLEASE PROVIDE PRIMITIVE ARGUMENT\n");
        printf("POSSIBLE VALUES: 'plane' 'sphere' 'cone' 'box' 'bezier'\n");
        return 1;
    }
    if(!strcmp(argv[1], "bezier")){
        if (argc == 5 && !strcmp(argv[1], "bezier"))
            write_bezier(argc, argv);
        else {
            fprintf(stderr, "Usage:%s bezier <tesselation> <infile> <outfile>\n", argv[0]);
            exit(1);
        }
        exit(0);
    }
    Primitive* tmp = argparse(argc, argv);
    if(tmp == NULL) return 1;

    int fd = open(argv[argc-1], O_CREAT | O_WRONLY, 0777);
    Serialize_model* m = new Serialize_model(
        Vertex::convert(tmp->get_vertices())
      , Vertex::convert(tmp->get_normals())
      , tmp->get_textures());
    auto serialized = m->serialize();

    write(fd, std::get<1>(serialized), std::get<0>(serialized));
    printf("Done.\n");
    return 0;
}

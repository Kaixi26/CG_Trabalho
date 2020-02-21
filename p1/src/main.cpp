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
        printf("WRONG NUMBER OF ARGUMENTS FOR PLANE.\nPLEASE USE 3 ARGUMENTS.\n");
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

Primitive* argparse(int argc, char* argv[]){
    switch((int)Primitive::type_parse(argv[1])){
        case PTYPE_PLANE:
            return argparse_plane(argc, argv);
        case PTYPE_ERR:
            printf("INVALID PRIMITIVE TYPE.\n");
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    Primitive* tmp = argparse(argc, argv);
    if(tmp == NULL) return 1;

    int fd = open(argv[argc-1], O_CREAT | O_WRONLY);
    printf("%s", argv[argc-1]);
    std::tuple<int,Vertex*> vertices = tmp->get_vertices();
    std::tuple<int, void*> serialized =
        Vertex::serialize_array(std::get<1>(vertices), std::get<0>(vertices));

    write(fd, std::get<1>(serialized), std::get<0>(serialized));
    delete(tmp);
    close(fd);
    printf("Done.\n");
    return 0;
}

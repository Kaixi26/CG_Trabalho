#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "../common/matrix.h"
#include "../common/vertex.h"
#include "../engine/model_rtree.h"
#include "../engine/tinyxml2/tinyxml2.h"

using namespace tinyxml2;

int main(int argc, char** argv){
    XMLDocument DOC;
    DOC.LoadFile(argv[1]);
    if(DOC.ErrorID()){
        printf("%s\n", DOC.ErrorStr());
        return DOC.ErrorID();
    }
    XMLNode* scene = DOC.FirstChildElement("scene");
    if(scene == NULL){
        printf("Could not find <scene>.");
        return -1;
    }

    Model_rtree* tmp = new Model_rtree(scene);

    tmp->print();
    //XMLNode* model = scene->FirstChild();
    //while(model){
    //    //if(!strcmp(model->Value(), "model"))
    //    //    printf("\t%s\n", model->ToElement()->Attribute("file"));
    //    printf("\t%s ", model->ToElement()->Name());
    //    printf("\t%f\n", model->ToElement()->FloatAttribute("X", 0));
    //    model = model->NextSibling();
    //}

}

//int main(){
//    Matrix test = Matrix::identity();
//    Vertex tmp = Vertex(1,2,3);
//    float v[4][4] = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
//    float v2[4][4] = {{0,0,0,0},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
//    Matrix t0 = Matrix(v);
//    Matrix t1 = Matrix(v2);
//    t0.mult(t1);
//    t0.print();
//    printf("\n");
//    return 0;
//}
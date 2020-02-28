#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <tuple>

#include "tinyxml2.h"
#include "vertex.h"

using namespace tinyxml2;

XMLDocument DOC;

void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio 
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void keyboard_handler(unsigned char key, int x, int y){
    switch(key){
        case 'r':
            glutPostRedisplay();
            break;
        case 'q':
            printf("Quitting program.");
            exit(0);
    }
}

void draw_axis(){
    glBegin(GL_LINES);
// X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f( 0.0f, 0.0f, 0.0f);
    glVertex3f( 100.0f, 0.0f, 0.0f);
// Y Axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
// Z Axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

void drawModel(XMLNode* modelNode){
    printf("%s\n", modelNode->ToElement()
           ->Attribute("file"));

    int fd = open(modelNode->ToElement()->Attribute("file"), O_RDONLY);
    if(fd == -1){
        printf("Failed opening.\n");
        return;
    }
    off_t fsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    void* buf = malloc(fsize + 1);
    read(fd, buf, fsize);
    close(fd);

    std::tuple<int, Vertex*> vertices = Vertex::parse_array(buf);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin(GL_TRIANGLES);
    for(int i=0; i<std::get<0>(vertices); i++){
        glColor3f((float)rand()/INT_MAX,
                  (float)rand()/INT_MAX,
                  (float)rand()/INT_MAX);
        Vertex tmp = std::get<1>(vertices)[i];
        //printf("Vertex (%f,%f,%f)\n", tmp.x, tmp.y, tmp.z);
        glVertex3f(tmp.x, tmp.y, tmp.z);
        //printf("%d\n", i);
    }
    //printf("%d\n", std::get<0>(vertices));
    glEnd();
}

void readAndDrawScene(){
    // clear buffers
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(5.0,5.0,5.0, 
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);

    XMLNode* scene = DOC.FirstChildElement("scene");
    if(scene == NULL){
        printf("Could not find <scene>.");
        return;
    }
    XMLNode* model = scene->FirstChild();
    printf("############## Starting to draw models. ##############\n");
    while(model){
        if(!strcmp(model->Value(), "model"))
            drawModel(model);
        model = model->NextSibling();
    }
    printf("############## Finished drawing models. ##############\n");

    draw_axis();
    glutSwapBuffers();
}

int main(int argc, char** argv){
    if(argc != 2){
        printf("Wrong number of arguments.");
        return 1;
    }
    DOC.LoadFile(argv[1]);
    if(DOC.ErrorID()){
        printf("%s\n", DOC.ErrorStr());
        return DOC.ErrorID();
    }
//  init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow("SCENE");
        
// Required callback registry 
    glutDisplayFunc(readAndDrawScene);
    glutKeyboardFunc(keyboard_handler);
	glutReshapeFunc(changeSize);

//  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
// enter GLUT's main cycle
    glutMainLoop();
    
    return 1;
}

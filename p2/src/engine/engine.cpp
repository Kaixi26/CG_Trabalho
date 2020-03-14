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
#include <time.h>

#include <tuple>

#include "../engine/tinyxml2/tinyxml2.h"
#include "../common/vertex.h"

using namespace tinyxml2;

const char* help_str = "List of commands:\n"
"\ts to reseed triangle random colors\n"
"\ta toggle drawing of axis\n"
"\tc toggle random/gray and white triangle colors\n"
"\tC alternate camera modes {explorer, free}\n"
"\tm alternate between polygon modes {triangle, line, point}\n"
"\tExplorer camera mode:\n"
"\t\th move camera to the left\n"
"\t\tj move camera down\n"
"\t\tk move camera up\n"
"\t\tl move camera to the right\n"
"\t\t+ move camera closer to center\n"
"\t\t- move camera further from center\n"
"\tFree camera mode:\n"
"\t\th move camera angle to the left\n"
"\t\tj move camera angle down\n"
"\t\tk move camera angle up\n"
"\t\tl move camera angle to the right\n"
"\t\tH move camera in the positive x direction\n"
"\t\tJ move camera in the negative z direction\n"
"\t\tK move camera in the positive z direction\n"
"\t\tL move camera in the negative x direction\n"
"\t\t+ move camera in the positive y direction\n"
"\t\t- move camera in the negative y direction\n"
;

XMLDocument DOC;

typedef enum camera_mode {
    EXPLORER,
    FREE
} camera_mode;

struct {
    int polygon_mode = GL_FILL;
    int seed = time(NULL);
    int rand_color = true;
    bool draw_axis = true;

    camera_mode cam_mode = EXPLORER;
    // EXPLORER MODE
    float cam_alpha = M_PI_4;
    float cam_beta = M_PI_4;
    float cam_radius = 5;

    // FREE MODE
    float cam_free_alpha = 0;
    float cam_free_beta = 0;
    float cam_free_x = 0;
    float cam_free_y = 0;
    float cam_free_z = 0;

} ENGINE_STATE;

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
        /* update polygon mode */
        case 'm':
            switch(ENGINE_STATE.polygon_mode){
                case GL_FILL:
                    ENGINE_STATE.polygon_mode = GL_LINE;
                    break;
                case GL_LINE:
                    ENGINE_STATE.polygon_mode = GL_POINT;
                    break;
                case GL_POINT:
                    ENGINE_STATE.polygon_mode = GL_FILL;
                    break;
            }
        /* update seed */
        case 's':
            ENGINE_STATE.seed = rand();
            break;
        case 'a':
            ENGINE_STATE.draw_axis = !ENGINE_STATE.draw_axis;
            break;
        case 'c':
            ENGINE_STATE.rand_color = !ENGINE_STATE.rand_color;
            break;
        case 'C':
            if(ENGINE_STATE.cam_mode == EXPLORER){
	            ENGINE_STATE.cam_free_x =
                    ENGINE_STATE.cam_radius*cos(ENGINE_STATE.cam_beta)*sin(ENGINE_STATE.cam_alpha);
			    ENGINE_STATE.cam_free_y =
                    ENGINE_STATE.cam_radius*sin(ENGINE_STATE.cam_beta);
			    ENGINE_STATE.cam_free_z =
                    ENGINE_STATE.cam_radius*cos(ENGINE_STATE.cam_beta)*cos(ENGINE_STATE.cam_alpha);
                ENGINE_STATE.cam_free_alpha = M_PI + ENGINE_STATE.cam_alpha;
                ENGINE_STATE.cam_free_beta  = - ENGINE_STATE.cam_beta;
                ENGINE_STATE.cam_mode = FREE;
            }
            else if(ENGINE_STATE.cam_mode == FREE){
                ENGINE_STATE.cam_mode = EXPLORER;
            }
            break;
		case 'h':
            if(ENGINE_STATE.cam_mode == EXPLORER)
			    ENGINE_STATE.cam_alpha -= 0.1;
            else if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_alpha += 0.05;
			break;
		case 'l':
            if(ENGINE_STATE.cam_mode == EXPLORER)
			    ENGINE_STATE.cam_alpha += 0.1;
            else if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_alpha -= 0.05;
			break;
		case 'j':
            if(ENGINE_STATE.cam_mode == EXPLORER){
                if(ENGINE_STATE.cam_beta - 0.05 > -M_PI_2)
			        ENGINE_STATE.cam_beta -= 0.05;
            }
            else if(ENGINE_STATE.cam_mode == FREE){
                if(ENGINE_STATE.cam_free_beta - 0.1 > -M_PI_2)
			        ENGINE_STATE.cam_free_beta -= 0.05;
            }
			break;
		case 'k':
            if(ENGINE_STATE.cam_mode == EXPLORER){
                if(ENGINE_STATE.cam_beta + 0.1 < M_PI_2)
			        ENGINE_STATE.cam_beta += 0.1;
            }
            else if(ENGINE_STATE.cam_mode == FREE)
                if(ENGINE_STATE.cam_free_beta + 0.05 < M_PI_2)
			        ENGINE_STATE.cam_free_beta += 0.05;
			break;
        case 'H':
            if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_x -= 0.1;
            break;
        case 'J':
            if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_z += 0.1;
            break;
        case 'K':
            if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_z -= 0.1;
            break;
        case 'L':
            if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_x += 0.1;
            break;
		case '+':
            if(ENGINE_STATE.cam_mode == EXPLORER)
			    ENGINE_STATE.cam_radius -= 0.1;
            else if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_y += 0.1;
			break;
		case '-':
            if(ENGINE_STATE.cam_mode == EXPLORER)
			    ENGINE_STATE.cam_radius += 0.1;
            else if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_y -= 0.1;
			break;
        default:
            return;
    }
    glutPostRedisplay();
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
    printf("\t%s\n", modelNode->ToElement()
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
        if(ENGINE_STATE.rand_color){
            if(i%3 == 0)
                glColor3f((float)rand()/INT_MAX, (float)rand()/INT_MAX, (float)rand()/INT_MAX);
        } else {
            if((i/3)%2 == 0) glColor3f(1, 1, 1);
            else if((i/3)%2 == 1) glColor3f(0.5,0.5,0.5);
        }
        Vertex tmp = std::get<1>(vertices)[i];
        //printf("Vertex (%f,%f,%f)\n", tmp.x, tmp.y, tmp.z);
        glVertex3f(tmp.x, tmp.y, tmp.z);
        //printf("%d\n", i);
    }
    //printf("%d\n", std::get<0>(vertices));
    glEnd();
}

void read_and_draw_scene(){
    srand(ENGINE_STATE.seed);
    // clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    if(ENGINE_STATE.cam_mode == EXPLORER)
        gluLookAt(ENGINE_STATE.cam_radius*cos(ENGINE_STATE.cam_beta)*sin(ENGINE_STATE.cam_alpha),
                  ENGINE_STATE.cam_radius*sin(ENGINE_STATE.cam_beta),
                  ENGINE_STATE.cam_radius*cos(ENGINE_STATE.cam_beta)*cos(ENGINE_STATE.cam_alpha),
                  0.0,0.0,0.0,
                  0.0f,1.0f,0.0f);
    else if(ENGINE_STATE.cam_mode == FREE){
        gluLookAt(ENGINE_STATE.cam_free_x, ENGINE_STATE.cam_free_y, ENGINE_STATE.cam_free_z,
                  ENGINE_STATE.cam_free_x + cos(ENGINE_STATE.cam_free_beta)*sin(ENGINE_STATE.cam_free_alpha),
                  ENGINE_STATE.cam_free_y + sin(ENGINE_STATE.cam_free_beta),
                  ENGINE_STATE.cam_free_z + cos(ENGINE_STATE.cam_free_beta)*cos(ENGINE_STATE.cam_free_alpha),
                  0.0f,1.0f,0.0f);
    }
    if(ENGINE_STATE.draw_axis) draw_axis();
    


    XMLNode* scene = DOC.FirstChildElement("scene");
    if(scene == NULL){
        printf("Could not find <scene>.");
        return;
    }
    XMLNode* model = scene->FirstChild();

    glPolygonMode(GL_FRONT, ENGINE_STATE.polygon_mode);

    printf("Starting to draw models.\n");
    while(model){
        if(!strcmp(model->Value(), "model"))
            drawModel(model);
        model = model->NextSibling();
    }
    printf("Finished drawing models.\n");

    glutSwapBuffers();
}

int main(int argc, char** argv){
    if(argc != 2){
        printf("Wrong number of arguments.");
        return 1;
    }
    if(!strcmp(argv[1], "--help")){
        printf("%s", help_str);
        return 0;
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
    glutCreateWindow("ENGINE");
        
// Required callback registry 
    glutDisplayFunc(read_and_draw_scene);
    glutKeyboardFunc(keyboard_handler);
	glutReshapeFunc(changeSize);

//  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
// enter GLUT's main cycle
    glutMainLoop();
    
    return 1;
}

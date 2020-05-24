#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <IL/il.h>

#include <math.h>
#include <stdio.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include <tuple>

#include "../engine/tinyxml2/tinyxml2.h"
#include "../engine/model_rtree.h"
#include "../common/vertex.h"
#include "../common/matrix.h"

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
    float cam_beta = M_PI_4/2;
    float cam_radius = 15;

    // FREE MODE
    float cam_free_alpha = 0;
    float cam_free_beta = 0;
    float cam_free_x = 0;
    float cam_free_y = 0;
    float cam_free_z = 0;

    Model_rtree* model_rtree = NULL;
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
			    ENGINE_STATE.cam_radius -= 0.2;
            else if(ENGINE_STATE.cam_mode == FREE)
                ENGINE_STATE.cam_free_y += 0.1;
			break;
		case '-':
            if(ENGINE_STATE.cam_mode == EXPLORER)
			    ENGINE_STATE.cam_radius += 0.2;
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


void set_camera(){
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
}

void render_scene(){
    // clear buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    set_camera();

    if(ENGINE_STATE.draw_axis) draw_axis();
    glColor3f(1.0f, 1.0f, 1.0f);

    glPolygonMode(GL_FRONT, ENGINE_STATE.polygon_mode);

    srand(ENGINE_STATE.seed);
    float t = glutGet(GLUT_ELAPSED_TIME)/(float)1000;
    ENGINE_STATE.model_rtree->draw(draw_opts{
          color : (ENGINE_STATE.rand_color ? DRAW_OPTS_COLOR_VBO : DRAW_OPTS_COLOR_DEFAULT)
        , t : t
    });

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

//  init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow("ENGINE");
        
// Required callback registry 
    glutDisplayFunc(render_scene);
    glutIdleFunc(render_scene);
    glutKeyboardFunc(keyboard_handler);
	glutReshapeFunc(changeSize);

//  OpenGL settings
	glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT, GL_LINE);

    glewInit();
    ilInit();

    XMLDocument doc;
    doc.LoadFile(argv[1]);
    if(doc.ErrorID()){
        printf("%s\n", doc.ErrorStr());
        return doc.ErrorID();
    }

    Model::initBuffers();
    ENGINE_STATE.model_rtree = new Model_rtree(doc.FirstChildElement("scene"));
    ENGINE_STATE.model_rtree->print();

    
// enter GLUT's main cycle
    glutMainLoop();
    
    return 1;
}

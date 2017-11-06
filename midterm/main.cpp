#include <math.h>
#include "object.h"

void animate();
void render();
void resizeFunction(int, int);

//float dot(glm::vec3, glm::vec3);

int currentWidth, currentHeight;
float rate = 2.5;
objects doll, route;
//char *roadPt;


float theta = 0;
//GLuint myIndex =0;
//GLubyte myLists[10];

int main(int argc, char* argv[]){

  char dollPath[] = "./data/doll.obj";
  char routePath[] = "./data/route_model.obj";

  currentWidth = 800;
  currentHeight = 600;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(currentWidth, currentHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gluLookAt(0 , 0, 1000, 0, 0, 0, 0, 1, 0);

  int windowHandle = glutCreateWindow("midterm");
  if(windowHandle < 1) {
    printf("ERROR: Could not create a new rendering window.\n");
    exit(EXIT_FAILURE);
  }

  GLenum GlewInitResult = glewInit();
  if(GlewInitResult != GLEW_OK ){
    fprintf(stderr,"ERROR: %s\n",glewGetErrorString(GlewInitResult));
    exit(EXIT_FAILURE);
  }


  glEnable(GL_DEPTH_TEST);
/*
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  {
    GLfloat MaterialAmbient[] = {1.0, 1.0, 1.0, 1.0f};
    GLfloat MaterialDiffuse[] = {1, 1, 1, 1.0f};
    GLfloat MaterialSpecular[] =  {1.2, 1.2, 1.2, 1.0f};
    GLfloat AmbientLightPosition[] = {0, 100, 0, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, MaterialAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, MaterialDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, MaterialSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, AmbientLightPosition);
  }


     myIndex =  glGenLists(1);
     glNewList(myIndex, GL_COMPILE); // compile the first one
     DrawCarModel();
     glEndList();
     myLists[0] = myIndex;
*/

  doll = getObjInfo(dollPath);
  //doll = getObjInfo(argv[1]);
  route = getObjInfo(routePath);

  calVerNormal(&doll, false);

  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(render);
  glutMainLoop();

  return 0;
}

void resizeFunction(int width, int height){
  currentWidth = width;
  currentHeight = height;
  glViewport(0, 0, width, height);
}
/*
float dot(glm::vec3 v1, glm::vec3 v2){

  float d = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  return d;
}
*/
void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-float(currentWidth)/rate, float(currentWidth)/rate,
      -float(currentHeight)/rate, float(currentHeight)/rate,
      -currentHeight * 10.0, currentHeight * 10.0);

//  glOrtho(-50, 50, -50, 50, -currentHeight * 10.0, currentHeight * 10.0);
/*
  theta += 0.05;
  gluLookAt(1000 * cos(theta) , 0, 1000 * sin(theta), 0, 0, 0, 0, 1, 0);
*/
  //gluLookAt(0 , 0, 1000, 0, 0, 0, 0, 1, 0);
  animate();

  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();

}

void animate(){
/*
  float A[] = {
    1, 0,  0, 0,
    0, 0, -1, 0,
    0, 1,  0, 0,
    0, 0,  0, 1
  };
*/
  glMatrixMode(GL_MODELVIEW);

  glPushMatrix();
    //glMultMatrixf(A);
    glRotatef(-90, 1, 0, 0);
    drawObj(doll, true);
  glPopMatrix();

  glPushMatrix();
    //glMultMatrixf(A);
    glRotatef(-90, 1, 0, 0);
    drawObj(route, false);
  glPopMatrix();

}

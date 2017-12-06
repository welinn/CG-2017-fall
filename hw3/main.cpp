#include "object.h"

void animate();
void render();
void resizeFunction(int, int);
//void rotRouteData();
//void mouseFunc(int, int, int, int);
//void mouseMotionFunc(int, int);
void timer(int);

int currentWidth, currentHeight;
int imgWidth = 400, imgHeight = 300;
objects buck, santa, venus;
//vector<glm::vec3> routeData;
//int step = 0;
//int totalLen;

glm::vec3 cameraPos(0, 50, 100);
//int roomAngle = 60;
//double cameraRotAngle;
//int mouseX, mouseY;
//bool leftButtonDown = false;
//bool rightButtonDown = false;
/*
void calAngle(){
  glm::vec3 vector(0, 1, 0);
//  if(cameraPos.z < 0) vector.z = -1;
  cameraRotAngle = acos(dot(cameraPos, vector));
  //if(cameraPos.z < 0) cameraRotAngle *= -1;
}
*/
int main(int argc, char* argv[]){

  char buckPath[] = "./data/Buck.obj";
  char santaPath[] = "./data/Santa Claus.obj";
  char venusPath[] = "./data/Venus_high_Stereo_withlogo.obj";

  currentWidth = 1080;
  currentHeight = 720;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(currentWidth, currentHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
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

  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  {
    GLfloat MaterialAmbient[] = {0.4, 0.4, 0.4, 1.0f};
    GLfloat MaterialDiffuse[] = {0.7, 0.7, 0.7, 1.0f};
    GLfloat MaterialSpecular[] =  {1.2, 1.2, 1.2, 1.0f};
    GLfloat AmbientLightPosition[] = {-50, 80, 0, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, MaterialAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, MaterialDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, MaterialSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, AmbientLightPosition);
  }

  buck = getObjInfo(buckPath);
  santa = getObjInfo(santaPath);
  venus = getObjInfo(venusPath);

  calVerNormal(&buck, false);
  calVerNormal(&santa, false);
  calVerNormal(&venus, false);

  //float t = buck.size.z / 2;
  rotateByVec(&buck, glm::vec3(0, 1, 0), 'z', glm::vec3(0, /*t*/0, 0), true);
  rotateByVec(&santa, glm::vec3(0, 1, 0), 'z', glm::vec3(0, 0, 0), true);
  rotateByVec(&venus, glm::vec3(0, 1, 0), 'z', glm::vec3(0, 0, -100), true);


  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(render);
  //glutMouseFunc(mouseFunc);
  //glutMotionFunc(mouseMotionFunc);
  //glutTimerFunc(20, timer, 0);
  glutMainLoop();

  return 0;
}

void resizeFunction(int width, int height){
  currentWidth = width;
  currentHeight = height;
  glViewport(0, 0, width, height);
}

void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //gluPerspective(60, (double)currentWidth / currentHeight, 5, 4000);
  glFrustum(-5, 5, -3, 3, 3, 2000);
  gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, 0, 0, 0, 0, 1, 0);
  glMatrixMode(GL_MODELVIEW);

  animate();

  glFlush();
  glutSwapBuffers();
  //glutPostRedisplay();

}

void animate(){

/*
  glm::vec3 vec = routeData[(step + 1) % totalLen] - routeData[step];
  glm::vec3 trans = routeData[(step + 1) % totalLen] - doll.position;
  rotateByVec(&doll, vec, 'z', trans);
  if(step % 5 == 0){
    glm::vec3 g = glm::vec3(0, -1, 0);
    glm::vec3 hori = cross(g, doll.axis.z);
    glm::vec3 top = cross(hori, doll.axis.z);
    float dot1, dot2;
    dot1 = acos(dot(top, doll.axis.y, true));
    dot2 = acos(dot(-top, doll.axis.y, true));
    if(dot1 < 0) dot1 += 2 * M_PI;
    if(dot2 < 0) dot2 += 2 * M_PI;
    if(dot1 < dot2) doll.axis.y = top;
    else doll.axis.y = -top;
  }
*/

  glPushMatrix();
    drawObj(buck, true);
    drawObj(venus, true);
  glPopMatrix();

  glPushMatrix();
    drawObj(santa, true);
  glPopMatrix();
/*
  step = ++step % totalLen;
*/
}
/*
void timer(int t){
  glutTimerFunc(50, timer, t + 1);
  glutPostRedisplay();
}*/

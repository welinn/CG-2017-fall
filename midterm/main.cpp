#include "object.h"

void animate();
void render();
void resizeFunction(int, int);
void rotRouteData();
void mouseFunc(int, int, int, int);
void mouseMotionFunc(int, int);

int currentWidth, currentHeight;
float rate = 3.7;
objects doll, route;
vector<glm::vec3> routeData;
int step = 0;
int totalLen;

int roomAngle = 60;
int mouseX, mouseY;
bool leftButtonDown = false;
bool rightButtonDown = false;


int main(int argc, char* argv[]){

  char dollPath[] = "./data/doll.obj";
  char routePath[] = "./data/route_model.obj";
  char routeDataPath[] = "./data/route.TXT";

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

  doll = getObjInfo(dollPath);
  route = getObjInfo(routePath);

  calVerNormal(&doll, false);

  float t = doll.size.z / 2;
  rotateByVec(&doll, glm::vec3(0, 1, 0), 'z', glm::vec3(0, t, 0), true);
  rotateByVec(&doll, glm::vec3(1, 0, 1), 'z', glm::vec3(0, 0, 0), true);
  rotateByVec(&route, glm::vec3(0, 1, 0), 'z', glm::vec3(0, 0, 0), true);

  FILE* routeDataPtr = fopen(routeDataPath, "r");
  if(routeDataPtr == NULL){
    printf("Open %s fail\n", routeDataPath);
    exit(EXIT_FAILURE);
  }
  glm::vec3 p;
  while(fscanf(routeDataPtr, "%f %f %f\n", &p.x, &p.y, &p.z) != EOF){
    routeData.push_back(p);
  }
  rotRouteData();
  totalLen = routeData.size();

  doll.position = routeData[0];

  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(render);
  glutMouseFunc(mouseFunc);
  glutMotionFunc(mouseMotionFunc);
  glutMainLoop();

  return 0;
}

void resizeFunction(int width, int height){
  currentWidth = width;
  currentHeight = height;
  glViewport(0, 0, width, height);
}

void mouseFunc(int button, int state, int x, int y){
//  printf("button: %d\n", button);
//  printf("state: %d\n", state);
  if(button == 3 && roomAngle > 5){
    roomAngle -= 2;
  }
  else if(button == 4 && roomAngle < 175){
    roomAngle += 2;
  }
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
    leftButtonDown = true;
    printf("01\n");
  }
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
    printf("02\n");
    leftButtonDown = false;
  }
  else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
    printf("03\n");
    rightButtonDown = true;
  }
  else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
    printf("04\n");
    rightButtonDown = false;
  }
}

void mouseMotionFunc(int x, int y){

}

void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
/*
  glOrtho(-float(currentWidth)/rate, float(currentWidth)/rate,
      -float(currentHeight)/rate, float(currentHeight)/rate,
      -currentHeight * 10.0, currentHeight * 10.0);
*/
  gluPerspective(roomAngle, (double)currentWidth / currentHeight, 5, 4000);
  gluLookAt(0 , 0, 500, 0, 0, 0, 0, 1, 0);
  glMatrixMode(GL_MODELVIEW);

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


  glPushMatrix();
    //glMultMatrixf(A);
    //glRotatef(test, 1, 0, 0);
    drawObj(doll, true);
  glPopMatrix();

  glPushMatrix();
    drawObj(route, false);
  glPopMatrix();

  step = ++step % totalLen;
}

void rotRouteData(){
  float d[] = {
    1,  0, 0, 0,
    0,  0, 1, 0,
    0, -1, 0, 0,
    0,  0, 0, 1};
  Mat rotMat = Mat(4, 4, CV_32F, d).clone();
  float p[] = {0, 0, 0, 1};
  Mat pt = Mat(4, 1, CV_32F, p).clone();

  int len = routeData.size();
  glm::vec3 move;
  move.x = route.oriCenter.x / 2;
  move.y = route.oriCenter.y / 2;
  move.z = route.oriCenter.z / 2;

  for(int i = 0; i < len; i++){
    routeData[i] -= move;

    pt.at<float>(0, 0) = routeData[i].x;
    pt.at<float>(1, 0) = routeData[i].y;
    pt.at<float>(2, 0) = routeData[i].z;
    Mat newPt = rotMat * pt;
    routeData[i].x = newPt.at<float>(0, 0);
    routeData[i].y = newPt.at<float>(1, 0);
    routeData[i].z = newPt.at<float>(2, 0);

  }
}

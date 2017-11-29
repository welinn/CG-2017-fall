#include "object.h"

void animate();
void render();
void resizeFunction(int, int);
void rotRouteData();


int currentWidth, currentHeight;
float rate = 3.7;
objects doll, route;
vector<glm::vec3> routeData;

float test = 0;
int step = 0;
int totalLen;
//GLuint myIndex =0;
//GLubyte myLists[10];

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
//  gluLookAt(0 , 0, 1000, 0, 0, 0, 0, 1, 0);
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

/*
     myIndex =  glGenLists(1);
     glNewList(myIndex, GL_COMPILE); // compile the first one
     DrawCarModel();
     glEndList();
     myLists[0] = myIndex;
*/

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
/*
  glOrtho(-float(currentWidth)/rate, float(currentWidth)/rate,
      -float(currentHeight)/rate, float(currentHeight)/rate,
      -currentHeight * 10.0, currentHeight * 10.0);
*/
  gluPerspective(60, (double)currentWidth / currentHeight, 5, 4000);
/*
  theta += 0.05;
  gluLookAt(1000 * cos(theta) , 0, 1000 * sin(theta), 0, 0, 0, 0, 1, 0);
*/
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

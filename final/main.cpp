#include "objects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "glm/ext.hpp"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void test();
void setVec3(glm::vec3*, Mat);
glm::vec3 cross(glm::vec3, glm::vec3);
void skyInit(vector<glm::vec3>*, vector< vector<int> >*, vector<glm::vec3>*, float);
void textureInit(int, vector<Mat>*, vector<Mat>*, char*);
void draw(vector<glm::vec3>, vector< vector<int> >, vector<glm::vec3>, GLuint*, float);
void animate();
void render();
void resizeFunction(int, int);
void mouseFunc(int, int, int, int);
void mouseMotionFunc(int, int);
void timer(int);

typedef struct {
  glm::vec3 pos;
  glm::vec3 up;
  glm::vec3 lookAt;
}cameraInfo;

int currentWidth, currentHeight;
cameraInfo camera;
int roomAngle = 60;
double cameraRotAngle = M_PI/2;
int mouseX, mouseY;
bool leftButtonDown = false;
bool rightButtonDown = false;

vector<glm::vec3> skyVertex;
vector< vector<int> > skyMesh;
vector<glm::vec3> skyNormal;
float radius = 100;

int textureSize = 512;
vector<Mat> textureF;
vector<Mat> textureR;
GLuint textureID[2];
int num;

void cameraInit(){
  camera.pos = glm::vec3 (0, 0, 500);
  camera.up = glm::vec3 (0, 1, 0);
  camera.lookAt = glm::vec3 (0, 0, 0);
}

int main(int argc, char* argv[]){
  char path[] = "./imgData";
  cameraInit();
  skyInit(&skyVertex, &skyMesh, &skyNormal, radius);
  textureInit(textureSize, &textureF, &textureR, path);
  //  glGenTextures(2, &textureID[0]);
  num = 0;

  currentWidth = 1080;
  currentHeight = 720;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(currentWidth, currentHeight);
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  int windowHandle = glutCreateWindow("final");
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
  GLfloat MaterialAmbient[] = {0.4, 0.4, 0.4, 1.0f};
  GLfloat MaterialDiffuse[] = {0.7, 0.7, 0.7, 1.0f};
  GLfloat MaterialSpecular[] =  {1.2, 1.2, 1.2, 1.0f};
  GLfloat AmbientLightPosition[] = {0, 0, 0, 1.0f};

  glLightfv(GL_LIGHT0, GL_AMBIENT, MaterialAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, MaterialDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, MaterialSpecular);
  glLightfv(GL_LIGHT0, GL_POSITION, AmbientLightPosition);


  GLfloat m_Ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat m_Diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
  GLfloat m_Specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat m_Emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  GLfloat m_fShininess = 1;

  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &m_Emissive[0]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &m_Ambient[0]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &m_Diffuse[0]);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &m_Specular[0]);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_fShininess);

  test();
  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(render);
  glutMouseFunc(mouseFunc);
  glutMotionFunc(mouseMotionFunc);
  glutTimerFunc(20, timer, 0);
  glutMainLoop();

  return 0;
}

void resizeFunction(int width, int height){
  currentWidth = width;
  currentHeight = height;
  glViewport(0, 0, width, height);
}

void mouseFunc(int button, int state, int x, int y){
  if(button == 3 && roomAngle > 5){
    roomAngle -= 2;
  }
  else if(button == 4 && roomAngle < 175){
    roomAngle += 2;
  }
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
    leftButtonDown = true;
    mouseX = x;
    mouseY = y;
  }
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
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

  if(leftButtonDown){
    //axis y
    double r = sqrt(camera.pos.x*camera.pos.x + camera.pos.y*camera.pos.y + camera.pos.z*camera.pos.z);
    double arcLen = (mouseX - x) * 2 * M_PI * r / currentWidth;
    double theta = arcLen / r;
    float data[] = {
      cos(theta), 0, sin(theta), 0,
      0, 1,          0, 0,
      -sin(theta), 0, cos(theta), 0,
      0, 0,          0, 1};
    Mat rotY(4, 4, CV_32F, data);

    //axis x
    double arcLen2 = 2 * M_PI * r * (y - mouseY) / currentHeight;
    double theta2 = arcLen2 / r;
    if(cameraRotAngle + theta2 < 0) theta2 = -cameraRotAngle;
    else if(cameraRotAngle + theta2 > M_PI) theta2 = M_PI - cameraRotAngle;
    cameraRotAngle += theta2;

    glm::vec3 cameraAxisX = cross(camera.up, camera.lookAt - camera.pos);
    float a = cameraAxisX.x, b = cameraAxisX.y, c = cameraAxisX.z;
    float COS = cos(theta2), SIN = sin(theta2);
    float data2[] = {
      (1 - a*a)*COS + a*a, -a*b*COS - c*SIN + a*b, -a*c*COS + b*SIN + a*c, 0,
      -a*b*COS + c*SIN + a*b,    (1 - b*b)*COS + b*b, -b*c*COS - a*SIN + b*c, 0,
      -a*c*COS - b*SIN + a*c, -b*c*COS + a*SIN + b*c,    (1 - c*c)*COS + c*c, 0,
      0,                      0,                      0, 1};
    Mat rotX(4, 4, CV_32F, data2);
    float pos[] = {camera.pos.x, camera.pos.y, camera.pos.z, 1};
    Mat cPos(4, 1, CV_32F, pos);
    float upV[] = {camera.up.x, camera.up.y, camera.up.z, 1};
    Mat cUpV(4, 1, CV_32F, upV);

    cPos = rotX * rotY * cPos;
    cUpV = rotX * rotY * cUpV;
    setVec3(&(camera.pos), cPos);
    setVec3(&(camera.up), cUpV);

    mouseX = x;
    mouseY = y;
  }
  else{
    printf("x: %d, y:%d\n", x, y);
  }
  glutPostRedisplay();
}

void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(roomAngle, (double)currentWidth / currentHeight, 5, 4000);
  gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z,
      camera.lookAt.x, camera.lookAt.y, camera.lookAt.z,
      camera.up.x, camera.up.y, camera.up.z);
  glMatrixMode(GL_MODELVIEW);

  animate();

  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();

}
void test(){

  //imshow("01", textureF[0]);
  //waitKey();
  glGenTextures(2, textureID);
  //printf("%d\n", textureID[0]);
  glBindTexture(GL_TEXTURE_2D, textureID[0]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // ( NEW )
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // ( NEW )
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0, GL_BGR_EXT , GL_UNSIGNED_BYTE, textureF[num].data);

  glBindTexture(GL_TEXTURE_2D, textureID[1]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // ( NEW )
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // ( NEW )
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0, GL_BGR_EXT , GL_UNSIGNED_BYTE, textureR[num].data);


}
void animate(){

  glPushMatrix();
  draw(skyVertex, skyMesh, skyNormal, textureID, radius);
  glPopMatrix();

  //++num %= 61;

}
void timer(int t){
  glutTimerFunc(50, timer, t + 1);
  glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////

void setVec3(glm::vec3* vec, Mat data){
  vec->x = data.at<float>(0, 0);
  vec->y = data.at<float>(1, 0);
  vec->z = data.at<float>(2, 0);
}

glm::vec3 cross(glm::vec3 v1, glm::vec3 v2){
  glm::vec3 n;
  n.x = v1.y * v2.z - v1.z * v2.y;
  n.y = v1.z * v2.x - v1.x * v2.z;
  n.z = v1.x * v2.y - v1.y * v2.x;
  float total = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
  n /= total;

  return n;
}

void skyInit(vector<glm::vec3> *vertex, vector< vector<int> >* mesh, vector<glm::vec3> *normal, float r){
  float theta = 0.1;
  float phi = 0;

  //vertex
  glm::vec3 tmp(0, 0, r);
  vertex->push_back(tmp);

  for(; theta < M_PI; theta += 0.1){
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);
    for(phi = 0; phi < 2*M_PI; phi += 0.1){
      tmp.x = r * sinTheta * cos(phi);
      tmp.y = r * sinTheta * sin(phi);
      tmp.z = r * cosTheta;
      vertex->push_back(tmp);
    }
  }
  vertex->push_back(glm::vec3(0, 0, -r));

  //mesh
  int xyCut = M_PI * 2 / 0.1;
  int zCut = M_PI / 0.1;
  int len = vertex->size() - 1;


  for(int i = 1; i <= xyCut; i++){
    vector<int> tmp2;
    tmp2.push_back(0);
    tmp2.push_back(0);
    if(i % xyCut == 0) tmp2.push_back(1);
    else tmp2.push_back(i + 1);
    tmp2.push_back(i);
//if(i % xyCut == 0) printf("dfffdddfffdsdsfsdf\n");
    mesh->push_back(tmp2);
  }
  for(int i = 1; i < len; i++){
    vector<int> tmp2;

    tmp2.push_back(i);
    if(i % xyCut == 0) tmp2.push_back(i - xyCut + 1);
    else tmp2.push_back(i + 1);
    tmp2.push_back(tmp2[1] + xyCut);
    tmp2.push_back(tmp2[0] + xyCut);
    if(tmp2[2] > len) tmp2[2] = len;
    if(tmp2[3] > len) tmp2[3] = len;
    mesh->push_back(tmp2);
  }

  //normal
  len = mesh->size();
  glm::vec3 v1, v2;
  for(int i = 0; i < len; i++){
    if(i < xyCut){
      //123
      v1 = vertex->at( mesh->at(i)[2] ) - vertex->at( mesh->at(i)[1] );
      v2 = vertex->at( mesh->at(i)[3] ) - vertex->at( mesh->at(i)[1] );
    }
    else{
      //012
      v1 = vertex->at( mesh->at(i)[1] ) - vertex->at( mesh->at(i)[0] );
      v2 = vertex->at( mesh->at(i)[2] ) - vertex->at( mesh->at(i)[0] );
    }
    normal->push_back(cross(v1, v2));
  }
}

void textureInit(int size, vector<Mat> *tF, vector<Mat> *tR, char *path){

  char p[20];
  Mat img;
  for(int i = 0; i < 61; i++){
    if(i < 10) sprintf(p, "%s/F00%d.JPG", path, i);
    else sprintf(p, "%s/F0%d.JPG", path, i);
    img = imread(p, 1);
    resize(img, img, Size(size, size), 0, 0, INTER_LINEAR);
    flip(img, img, 0);
    tF->push_back(img);

    if(i < 10) sprintf(p, "%s/R00%d.JPG", path, i);
    else sprintf(p, "%s/R0%d.JPG", path, i);
    img = imread(p, 1);
    resize(img, img, Size(size, size), 0, 0, INTER_LINEAR);
    flip(img, img, 0);
    tR->push_back(img);
  }
}

void draw(vector<glm::vec3> vertex, vector< vector<int> > mesh, vector<glm::vec3> normal, GLuint *ID, float r){

  int len = mesh.size();
  for(int i = 0; i < len/*125*/; i++){

    glEnable(GL_TEXTURE_2D);

    if(vertex[ mesh[i][0] ].z < 0) glBindTexture(GL_TEXTURE_2D, ID[0]);
    else glBindTexture(GL_TEXTURE_2D, ID[1]);

    glBegin(GL_QUADS);
    glNormal3fv(&(normal[i].x));
    glTexCoord2f((vertex[ mesh[i][0] ].x - 0.08) * 0.8 / (2*r) + 0.08, (vertex[ mesh[i][0] ].y - 0.1) * 0.8 / (2*r) + 0.1);
    glVertex3fv(&(vertex[ mesh[i][0] ].x));
    glTexCoord2f((vertex[ mesh[i][1] ].x - 0.08) * 0.8 / (2*r) + 0.08, (vertex[ mesh[i][1] ].y - 0.1) * 0.8 / (2*r) + 0.1);
    glVertex3fv(&(vertex[ mesh[i][1] ].x));
    glTexCoord2f((vertex[ mesh[i][2] ].x - 0.08) * 0.8 / (2*r) + 0.08, (vertex[ mesh[i][2] ].y - 0.1) * 0.8 / (2*r) + 0.1);
    glVertex3fv(&(vertex[ mesh[i][2] ].x));
    glTexCoord2f((vertex[ mesh[i][3] ].x - 0.08) * 0.8 / (2*r) + 0.08, (vertex[ mesh[i][3] ].y - 0.1) * 0.8 / (2*r) + 0.1);
    glVertex3fv(&(vertex[ mesh[i][3] ].x));
    glEnd();

    glDeleteTextures(2, ID);
    glDisable(GL_TEXTURE_2D);
  }
}

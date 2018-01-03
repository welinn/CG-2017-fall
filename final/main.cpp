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

void animate();
void render();
void resizeFunction(int, int);
void mouseFunc(int, int, int, int);
void mouseMotionFunc(int, int);
void keyboardFunc(unsigned char, int, int);
void timer(int);
/////////////////////////////////////////////////////
void setVec3(glm::vec3*, Mat);
glm::vec3 cross(glm::vec3, glm::vec3);
void skyInit(vector<glm::vec3>*, vector< vector<int> >*, vector<glm::vec3>*, float);
void textureInit(int, vector<Mat>*, vector<Mat>*, char*);
void draw(vector<glm::vec3>, vector< vector<int> >, vector<glm::vec3>, GLuint*, float);
float calDist(glm::vec3, glm::vec3);
float calLen(glm::vec3);


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
bool videoPlay = true;

vector<glm::vec3> skyVertex;
vector< vector<int> > skyMesh;
vector<glm::vec3> skyNormal;
float radius = 10000;

int textureSize = 512;
vector<Mat> textureF;
vector<Mat> textureR;
float u1 = 0.167, v1 = 0.14;
float u2 = 0.169, v2 = 0.08;
float uHalfSize1 = 0.4, vHalfSize1 = 0.36;
float uHalfSize2 = 0.4, vHalfSize2 = 0.41;
GLuint textureID[2];
int num;


void cameraInit(){
  camera.pos = glm::vec3 (0, 0, 5000);
  camera.up = glm::vec3 (0, 1, 0);
  camera.lookAt = glm::vec3 (0, 0, 0);
}

int main(int argc, char* argv[]){
  char path[] = "./imgData";
  cameraInit();
  skyInit(&skyVertex, &skyMesh, &skyNormal, radius);
  textureInit(textureSize, &textureF, &textureR, path);
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
/*
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
*/
//  glGenTextures(2, &textureID[0]);

  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(render);
  glutMouseFunc(mouseFunc);
  glutMotionFunc(mouseMotionFunc);
  glutKeyboardFunc(keyboardFunc);
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
  if(button == 3){
    if(calDist(camera.pos, glm::vec3(0, 0, 0)) > 106){
      glm::vec3 tran = camera.lookAt - camera.pos;
      tran /= calLen(tran) / 100;
      camera.pos += tran;
    }
  }
  else if(button == 4){
    glm::vec3 tran = camera.lookAt - camera.pos;
    tran /= calLen(tran) / 100;
    camera.pos -= tran;
  }
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
    leftButtonDown = true;
    mouseX = x;
    mouseY = y;
  }
  else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
    leftButtonDown = false;
  }
}

void mouseMotionFunc(int x, int y){

  if(leftButtonDown){
    //axis y
    double r = calDist(camera.pos, camera.lookAt);
    double arcLen = (mouseX - x) * 2 * M_PI * r / currentWidth;
    if(calDist(camera.pos, glm::vec3(0, 0, 0)) < radius) arcLen *= -1;
    double theta = arcLen / r;
    float data[] = {
      cos(theta), 0, sin(theta), 0,
      0, 1,          0, 0,
      -sin(theta), 0, cos(theta), 0,
      0, 0,          0, 1};
    Mat rotY(4, 4, CV_32F, data);

    //axis x
    double arcLen2 = 2 * M_PI * r * (y - mouseY) / currentHeight;
    if(calDist(camera.pos, glm::vec3(0, 0, 0)) < radius) arcLen2 *= -1;
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

void keyboardFunc(unsigned char key, int x, int y){
  printf("keyboard: %d\n", key);
  if(key == 32) videoPlay = !videoPlay; //space
  else if(key == 'r' || key == 'R'){
    cameraInit();
    glutPostRedisplay();
  }
}

void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(roomAngle, (double)currentWidth / currentHeight, 5, 25000);

  glm::vec3 lookAt = camera.lookAt - camera.pos;
  lookAt /= calLen(lookAt) / radius;
  lookAt += camera.pos;
  gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z,
      lookAt.x, lookAt.y, lookAt.z,
      camera.up.x, camera.up.y, camera.up.z);
  glMatrixMode(GL_MODELVIEW);

  animate();

  glFlush();
  glutSwapBuffers();
  glutPostRedisplay();

}

void animate(){

  glPushMatrix();
    glGenTextures(2, textureID);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // ( NEW )
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // ( NEW )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0, GL_BGR_EXT , GL_UNSIGNED_BYTE, textureF[num].data);

    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // ( NEW )
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // ( NEW )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize, textureSize, 0, GL_BGR_EXT , GL_UNSIGNED_BYTE, textureR[num].data);

    draw(skyVertex, skyMesh, skyNormal, textureID, radius);
    glDeleteTextures(2, textureID);

  glPopMatrix();

  if(videoPlay) ++num %= 61;
}

void timer(int t){
  glutTimerFunc(50, timer, t + 1);
  glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////
float calDist(glm::vec3 p1, glm::vec3 p2){
  return calLen(p1 - p2);
}

float calLen(glm::vec3 v){
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

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
  float x1, x2, x3, x4, y1, y2, y3, y4;

  for(int i = 0; i < len; i++){
    glEnable(GL_TEXTURE_2D);

    if(vertex[ mesh[i][0] ].z < 0){
      glBindTexture(GL_TEXTURE_2D, ID[0]);
      x1  = ((-vertex[ mesh[i][0] ].x + r) * uHalfSize1 / r + u1);
      x2  = ((-vertex[ mesh[i][1] ].x + r) * uHalfSize1 / r + u1);
      x3  = ((-vertex[ mesh[i][2] ].x + r) * uHalfSize1 / r + u1);
      x4  = ((-vertex[ mesh[i][3] ].x + r) * uHalfSize1 / r + u1);
      y1 = (vertex[ mesh[i][0] ].y + r) * vHalfSize1 / r + v1;
      y2 = (vertex[ mesh[i][1] ].y + r) * vHalfSize1 / r + v1;
      y3 = (vertex[ mesh[i][2] ].y + r) * vHalfSize1 / r + v1;
      y4 = (vertex[ mesh[i][3] ].y + r) * vHalfSize1 / r + v1;
    }
    else{
      glBindTexture(GL_TEXTURE_2D, ID[1]);
      x1 = (vertex[ mesh[i][0] ].x + r) * uHalfSize2 / r + u2;
      x2 = (vertex[ mesh[i][1] ].x + r) * uHalfSize2 / r + u2;
      x3 = (vertex[ mesh[i][2] ].x + r) * uHalfSize2 / r + u2;
      x4 = (vertex[ mesh[i][3] ].x + r) * uHalfSize2 / r + u2;
      y1 = (vertex[ mesh[i][0] ].y + r) * vHalfSize2 / r + v2;
      y2 = (vertex[ mesh[i][1] ].y + r) * vHalfSize2 / r + v2;
      y3 = (vertex[ mesh[i][2] ].y + r) * vHalfSize2 / r + v2;
      y4 = (vertex[ mesh[i][3] ].y + r) * vHalfSize2 / r + v2;
    }
    glBegin(GL_QUADS);
      glNormal3fv(&(normal[i].x));
      glTexCoord2f(x1, y1);  glVertex3fv(&(vertex[ mesh[i][0] ].x));
      glTexCoord2f(x2, y2);  glVertex3fv(&(vertex[ mesh[i][1] ].x));
      glTexCoord2f(x3, y3);  glVertex3fv(&(vertex[ mesh[i][2] ].x));
      glTexCoord2f(x4, y4);  glVertex3fv(&(vertex[ mesh[i][3] ].x));
    glEnd();

    glDisable(GL_TEXTURE_2D);
  }
}

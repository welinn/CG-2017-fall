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

glm::vec3 cross(glm::vec3, glm::vec3);
float dot(glm::vec3, glm::vec3, bool = true);
void setVec3(glm::vec3*, Mat);
//=====^^=====
//    math

typedef struct{
  float r;
  float g;
  float b;
}vColor;

typedef struct{
  glm::vec3 x;
  glm::vec3 y;
  glm::vec3 z;
}localCoordinate;

typedef struct {
  vector<int> vertexIndices;
  vector<int> uvIndices;
  vector<int> normalIndices;
}objMeshs;

typedef struct {

  vector<glm::vec3> verNormal;
  vector<vColor> verColor;
  vector<glm::vec3> vertices;
  vector<glm::vec2> uvs;
  vector<glm::vec3> normals;
  vector<objMeshs> g;

  localCoordinate axis;
  glm::vec3 position;

  glm::vec3 oriCenter;
  glm::vec3 size;

}objects;

objects getObjInfo(char*);
void drawObj(objects, bool);
void moveToOrigin(objects*);
void calVerNormal(objects*, bool);
void rotateByVec(objects*, glm::vec3, char, glm::vec3 = glm::vec3(0, 0, 0), bool = false);
void rotation(objects*, float); //TODO
//void translation(objects*, glm::vec3);

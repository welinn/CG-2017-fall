#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "glm/ext.hpp"

using namespace std;

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
  localCoordinate axis;
  glm::vec3 origin;
}objMeshs;

typedef struct {

  vector<glm::vec3> verNormal;
  vector<vColor> verColor;
  vector<glm::vec3> vertices;
  vector<glm::vec2> uvs;
  vector<glm::vec3> normals;
  vector<objMeshs> g;

}objects;

objects getObjInfo(char*);
void drawObj(objects, bool);
void moveToOrigin(objects*);//TODO
void calVerNormal(objects*, bool);

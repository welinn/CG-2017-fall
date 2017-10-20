#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>

#include "glm/ext.hpp"

using namespace std;

typedef struct {
  unsigned int vertexIndices[9999];
  unsigned int uvIndices[9999];
  unsigned int normalIndices[9999];
  int vi;
  int uvi;
  int ni;

}objMeshs;

typedef struct {
  vector<glm::vec3> vertices;
  vector<glm::vec2> uvs;
  vector<glm::vec3> normals;
  vector<objMeshs> g;
}objects;

objects getObjInfo(char*);

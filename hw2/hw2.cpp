#include <math.h>
#include "loadObj.h"

void animate();
void resizeFunction(int, int);
void drawObj();

int currentWidth, currentHeight;
objects obj;
//GLuint myIndex =0;
//GLubyte myLists[10];

int main(int argc, char* argv[]){

  //glutInit(&argc, argv);
  if(argc < 2){
    printf("No input obj file (argc < 2, format: exe_file input_obj_file)\n");
    return 1;
  }

  currentWidth = 600;
  currentHeight = 400;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(currentWidth, currentHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gluLookAt(0 , 0, 1000, 50, 50, 0, 0, 1, 0);

  int windowHandle = glutCreateWindow("HW2");
  if(windowHandle < 1) {
    printf("ERROR: Could not create a new rendering window.\n");
    exit(EXIT_FAILURE);
  }

  GLenum GlewInitResult = glewInit();
  if(GlewInitResult != GLEW_OK ){
    fprintf(stderr,"ERROR: %s\n",glewGetErrorString(GlewInitResult));
    exit(EXIT_FAILURE);
  }

  /*
     glEnable(GL_DEPTH_TEST);//<---
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     {
     GLfloat MaterialAmbient[] = {0.4,0.4,0.4,1.0f};
     GLfloat MaterialDiffuse[] = {0.7,0.7,0.7,1.0f};
     GLfloat MaterialSpecular[] =  { 1.2,1.2,1.2, 1.0f};
     GLfloat AmbientLightPosition[] = {1000,1000,1000,1.0f};

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

  obj = getObjInfo(argv[1]);
  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(animate);
  glutMainLoop();

  return 0;
}

void resizeFunction(int width, int height){
  currentWidth = width;
  currentHeight = height;
  glViewport(0, 0, width, height);
}

void drawObj(){
  //char path[] = "./objs/box.obj";
  //objects obj = loadObj(path);
  int allObj = obj.g.size();

  for(int i = 0; i < allObj; i++){
    int allMesh = obj.g[i].vi;
    for(int j = 0; j < allMesh; j += 3){
/*
      glBegin(GL_TRIANGLES);
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 1]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 2]-1 ].x));
      glEnd();
*/
      glBegin(GL_LINES);
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 1]-1 ].x));
      glEnd();

      glBegin(GL_LINES);
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 1]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 2]-1 ].x));
      glEnd();

      glBegin(GL_LINES);
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 2]-1 ].x));
      glEnd();

    }
  }
}

void cross(float *n, glm::vec3 v1, glm::vec3 v2){
  n[0] = v1.y * v2.z - v1.z * v2.y;
  n[1] = v1.z * v2.x - v1.x * v2.z;
  n[2] = v1.x * v2.y - v1.y * v2.x;
}

float dot(glm::vec3 v1, glm::vec3 v2){

  float d = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  return d;
}

void calculateVolume(){

  int allObj = obj.g.size();
  glm::vec3 n(1/sqrt(2), 1/sqrt(2), 0);

  for(int i = 0; i < allObj; i++){
    int allMesh = obj.g[i].vi;

    float normal[3];
    float volume = 0;
    glm::vec3 p1, p2, p3, v1, v2;
    for(int j = 0; j < allMesh; j += 3){
      p1 = obj.vertices[ obj.g[i].vertexIndices[j]-1 ];
      p2 = obj.vertices[ obj.g[i].vertexIndices[j+1]-1 ];
      p3 = obj.vertices[ obj.g[i].vertexIndices[j+2]-1 ];
      v1 = p2 - p1;
      v2 = p3 - p1;

      cross(normal, v1, v2);

      //將點投影到xy平面算底面積
      v1.z = v2.z = 0;
      float projDot = dot(v1, v2);
      float area = 0.5 * sqrt((v1.x*v1.x + v1.y*v1.y) * (v2.x*v2.x + v2.y*v2.y) - projDot * projDot);

      float h = (p1.z + p2.z + p3.z) / 3;
      area *= h;
      //以法向量 n.z 的正負決定體積正負
      if(normal[2] < 0) area *= -1;
      volume += area;
    }
    printf("Object %d volume = %f\n", i, volume);
  }
}

void animate(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-float(currentWidth)/2.0, float(currentWidth)/2.0,
      -float(currentHeight)/2.0,float(currentHeight)/2.0,
      -currentHeight * 10.0, currentHeight * 10.0);

  calculateVolume();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
    drawObj();
  glPopMatrix();

  glFlush();
  glutSwapBuffers();
}

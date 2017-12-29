#include "object.h"

void render();
void resizeFunction(int, int);
void mouseFunc(int, int, int, int);
void saveImg(bool);

int currentWidth, currentHeight;
int imgWidth;
int imgNum = 1;
objects buck, santa, venus;
glm::vec3 cameraPos(0, 50, 100);
bool saving = false;

int main(int argc, char* argv[]){

  char buckPath[] = "./data/Buck.obj";
  char santaPath[] = "./data/Santa Claus.obj";
  char venusPath[] = "./data/Venus_high_Stereo_withlogo.obj";

  currentWidth = imgWidth = 1024;
  currentHeight = 768;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(currentWidth, currentHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  int windowHandle = glutCreateWindow("hw3");
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

  rotateByVec(&buck, glm::vec3(0, 1, 0), 'z', glm::vec3(-60, 0, 0), true);
  rotateByVec(&santa, glm::vec3(0, 1, 0), 'z', glm::vec3(60, 0, 0), true);
  rotateByVec(&venus, glm::vec3(0, 1, 0), 'z', glm::vec3(0, 0, -400), true);

  printf("Use mouse left button to enter output image size\n");
  printf("Use mouse right button to save screenshot\n");

  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(render);
  glutMouseFunc(mouseFunc);
  glutMainLoop();

  return 0;
}

void resizeFunction(int width, int height){
  currentWidth = width;
  currentHeight = height;
  glViewport(0, 0, width, height);
}

void mouseFunc(int button, int state, int x, int y){
  if(state == GLUT_DOWN){
    if(button == GLUT_LEFT_BUTTON){
      printf("Enter width: ");
      scanf("%d", &imgWidth);
      glutPostRedisplay();
    }
    else if(button == GLUT_RIGHT_BUTTON){
      saving = true;
      printf("Saving image...\n");
      glutPostRedisplay();
    }
    printf("Use mouse left button to enter output image size\n");
    printf("Use mouse right button to save screenshot\n");
  }
}

void saveImg(bool save){
  static std::vector<Mat> dstData;
  if(save){
    char path[20];
    int imgHeight = imgWidth * currentHeight / currentWidth;
    int len = dstData.size();
    int rate = sqrt(len);
    int x, y;
    Mat dst(currentHeight * rate, currentWidth * rate, CV_8UC3);
    Rect roi(0, 0, currentWidth, currentHeight);

    sprintf(path, "./dst%d.jpg", imgNum++);
    y = -currentWidth;
    for(int i = 0; i < len ; i++){
      if(i % rate == 0){
        y += currentWidth;
        x = currentHeight * (rate - 1);
      }
      roi.x = y;
      roi.y = x;
      dstData[i].copyTo(dst(roi));
      x -= currentHeight;

    }
    resize(dst, dst, Size(imgWidth, imgHeight), 0, 0, CV_INTER_AREA);
    imwrite(path, dst);
    printf("Image saved\n");
    dstData.clear();
  }
  else{
    Mat dst(currentHeight, currentWidth, CV_8UC3);
    glReadPixels(0, 0, currentWidth, currentHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, dst.data);
    flip(dst, dst, 0);
    dstData.push_back(dst);
  }
}

void render(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, currentWidth, currentHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float frustumWidth = (float) currentWidth / 200;
  float frustumHeight = (float) currentHeight / 200;

  if(!saving){
    glFrustum(-frustumWidth, frustumWidth, -frustumHeight, frustumHeight, 3, 2000);
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, 0, 0, 0, 0, 1, 0);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
      drawObj(buck, true);
      drawObj(santa, true);
      drawObj(venus, true);
    glPopMatrix();
  }

  else{
    float tmpWidth;
    float tmpHeight;
    int rate = ceil((float)imgWidth / currentWidth);
    float bufferSizeW = frustumWidth * 2 / rate;
    float bufferSizeH = frustumHeight * 2 / rate;
    tmpWidth = -frustumWidth;
    for(int i = 0; i < rate; i++){
      tmpHeight = -frustumHeight;
      for(int j = 0; j < rate; j++){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, currentWidth, currentHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(tmpWidth, tmpWidth + bufferSizeW, tmpHeight, tmpHeight + bufferSizeH, 3, 2000);
        gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, 0, 0, 0, 0, 1, 0);
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
          drawObj(buck, true);
          drawObj(santa, true);
          drawObj(venus, true);
          saveImg(false);
        glPopMatrix();
        tmpHeight += bufferSizeH;
      }
      tmpWidth += bufferSizeW;
    }
    saveImg(true);

    saving = false;
  }
  glFlush();
  glutSwapBuffers();

}

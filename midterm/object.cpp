//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
#include "object.h"

glm::vec3 cross(glm::vec3 v1, glm::vec3 v2){
  glm::vec3 n;
  n.x = v1.y * v2.z - v1.z * v2.y;
  n.y = v1.z * v2.x - v1.x * v2.z;
  n.z = v1.x * v2.y - v1.y * v2.x;
  float total = sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
  n /= total;

  return n;
}

float dot(glm::vec3 v1, glm::vec3 v2, bool normalize){
  float d = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  if(normalize){
    float len = sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z) * sqrt(v2.x*v2.x + v2.y*v2.y + v2.z*v2.z);
    d /= len;
  }
  return d;
}
//      math
// ======^^======
// ======vv======
//     object
objects getObjInfo(char* path){

  FILE* file = fopen(path, "r");
  if( file == NULL ){
    printf("Open file fault\n");
    exit(EXIT_FAILURE);
  }

  objMeshs meshs;
  objects obj ;
  char input[128];
  bool first = true;
  bool newMeshs = false;
  glm::vec3 verNInit;

  verNInit.x = verNInit.y = verNInit.z = 0;

  while(fscanf(file, "%s", input) != EOF){
    //vertex
    if(strcmp(input, "v") == 0){

      glm::vec3 vertex;
      fscanf(file, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
      obj.vertices.push_back(vertex);
      obj.verNormal.push_back(verNInit);

      char str[100];
      char *ptr = str;
      vColor vc;

      fscanf(file, "%s", str);
      int len = strlen(str);
      float value = atof(str);

      if(value != 0 || str[0] == '0' || (str[0] == '-' && str[1] == '0')){
        vc.r = value;
        fscanf(file, "%f %f", &vc.g, &vc.b);
      }
      else{
        fseek(file, -len, SEEK_CUR);
        vc.r = -1;
        vc.g = -1;
        vc.b = -1;
      }
      obj.verColor.push_back(vc);

    }
    //uv
    else if(strcmp(input, "vt") == 0){
      glm::vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y);
      obj.uvs.push_back(uv);
    }
    //normal vector
    else if(strcmp(input, "vn") == 0){
      glm::vec3 normal;
      fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
      obj.normals.push_back(normal);
    }
    //mesh
    else if(strcmp(input, "f") == 0){
      int value;
      int i, j;
      char str[100], *tmp;
      bool nData = false;

      for(i = 0; i < 3; i++){
        fscanf(file, "%s", str);
        tmp = strtok(str, "/");
        j = 0;
        while(tmp != NULL){
          value = atoi(tmp);
          if(j == 0) meshs.vertexIndices.push_back(value);
          else if(j == 1) meshs.uvIndices.push_back(value);
          else if(j == 2){
            meshs.normalIndices.push_back(value);
            nData = true;
          }
          tmp = strtok(NULL, "/");
          j++;
        }
        if(j == 1){
          meshs.uvIndices.push_back(-1);
          if(!nData && i == 2) meshs.normalIndices.push_back(-1);
        }
        else if(!nData && i == 2 && j == 2) meshs.normalIndices.push_back(-1);
      }

    }
    //different obj
    else if(strcmp(input, "g") == 0){
      if(first) first = false;
      else{
        newMeshs = true;
        obj.g.push_back(meshs);
      }
    }
  }
  obj.g.push_back(meshs);
  moveToOrigin(&obj);
  return obj;
}

void calMeshNormal(objects* obj){

  int allObj = obj->g.size();

  for(int i = 0; i < allObj; i++){
    int allMesh = obj->g[i].vertexIndices.size();
    glm::vec3 p1, p2, p3, v1, v2;
    for(int j = 0; j < allMesh; j += 3){
      p1 = obj->vertices[ obj->g[i].vertexIndices[j]-1 ];
      p2 = obj->vertices[ obj->g[i].vertexIndices[j+1]-1 ];
      p3 = obj->vertices[ obj->g[i].vertexIndices[j+2]-1 ];
      v1 = p2 - p1;
      v2 = p3 - p1;

      obj->normals.push_back(cross(v1, v2));
      int len = obj->normals.size();
      obj->g[i].normalIndices[j/3] = len;
    }
  }
}

void calVerNormal(objects *obj, bool meshNormal){

  if(!meshNormal){
    calMeshNormal(obj);
  }

  int allObj = obj->g.size();
  int allVertex = obj->vertices.size();
  int freq[allVertex];


  for(int i = 0; i < allObj; i++){
    for(int ii = 0; ii < allVertex; ii++) freq[ii] = 0;
    int allMesh = obj->g[i].vertexIndices.size();

    vector<glm::vec3> meshN;
    for(int j = 0; j < allMesh; j++){

      freq[ obj->g[i].vertexIndices[j]-1 ]++;
      obj->verNormal[ obj->g[i].vertexIndices[j]-1 ] += obj->normals[j/3];
    }
    for(int k = 0; k < allVertex; k++){
      if(freq[k] != 0) obj->verNormal[k] /= freq[k];
    }

  }

}

void moveToOrigin(objects *obj){

  glm::vec3 move;
  int len = obj->vertices.size();
  float xMax, xMin, yMax, yMin, zMax, zMin;

  xMin = xMax = obj->vertices[0].x;
  yMin = yMax = obj->vertices[0].y;
  zMin = zMax = obj->vertices[0].z;

  obj->position = glm::vec3(0, 0, 0);
  obj->axis.x = glm::vec3(1, 0, 0);
  obj->axis.y = glm::vec3(0, 1, 0);
  obj->axis.z = glm::vec3(0, 0, 1);

  for(int i = 0; i < len; i++){
    if(xMin > obj->vertices[i].x) xMin = obj->vertices[i].x;
    if(yMin > obj->vertices[i].y) yMin = obj->vertices[i].y;
    if(zMin > obj->vertices[i].z) zMin = obj->vertices[i].z;
    if(xMax < obj->vertices[i].x) xMax = obj->vertices[i].x;
    if(yMax < obj->vertices[i].y) yMax = obj->vertices[i].y;
    if(zMax < obj->vertices[i].z) zMax = obj->vertices[i].z;

  }

  obj->size.x = xMax - xMin;
  obj->size.y = yMax - yMin;
  obj->size.z = zMax - zMin;

  obj->oriCenter.x = xMax + xMin;
  obj->oriCenter.y = yMax + yMin;
  obj->oriCenter.z = zMax + zMin;
  move.x = obj->oriCenter.x / 2;
  move.y = obj->oriCenter.y / 2;
  move.z = obj->oriCenter.z / 2;

  for(int i = 0; i < len; i++) obj->vertices[i] -= move;
}

void drawObj(objects obj, bool color){

  int allObj = obj.g.size();
  vColor *color1, *color2, *color3;
  Mat oriP1(4, 1, CV_32F);
  Mat oriP2(4, 1, CV_32F);
  Mat oriP3(4, 1, CV_32F);
  float data[] = {
    obj.axis.x.x, obj.axis.y.x, obj.axis.z.x, obj.position.x,
    obj.axis.x.y, obj.axis.y.y, obj.axis.z.y, obj.position.y,
    obj.axis.x.z, obj.axis.y.z, obj.axis.z.z, obj.position.z,
               0,            0,            0,              1};
  Mat rotMat = Mat(4, 4, CV_32F, data).clone();
  float p1[3], p2[3], p3[3];

  oriP1.at<float>(3, 0) = oriP2.at<float>(3, 0) = oriP3.at<float>(3, 0) = 1;
  for(int i = 0; i < allObj; i++){
    int allMesh = obj.g[i].vertexIndices.size();
    for(int j = 0; j < allMesh; j += 3){
      color1 = &obj.verColor[obj.g[i].vertexIndices[j]-1];
      color2 = &obj.verColor[obj.g[i].vertexIndices[j + 1]-1];
      color3 = &obj.verColor[obj.g[i].vertexIndices[j + 2]-1];


      for(int ii = 0; ii < 3; ii++){
        oriP1.at<float>(ii, 0) = *(&obj.vertices[ obj.g[i].vertexIndices[j]-1 ].x + ii);
        oriP2.at<float>(ii, 0) = *(&obj.vertices[ obj.g[i].vertexIndices[j+1]-1 ].x + ii);
        oriP3.at<float>(ii, 0) = *(&obj.vertices[ obj.g[i].vertexIndices[j+2]-1 ].x + ii);
      }

      Mat newP1 = rotMat * oriP1;
      Mat newP2 = rotMat * oriP2;
      Mat newP3 = rotMat * oriP3;
      for(int ii = 0; ii < 3; ii++){
        p1[ii] = newP1.at<float>(ii, 0);
        p2[ii] = newP2.at<float>(ii, 0);
        p3[ii] = newP3.at<float>(ii, 0);
      }

      glBegin(GL_TRIANGLES);
        if(color){
          glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
          glNormal3fv(&(obj.verNormal[ obj.g[i].vertexIndices[j]-1 ].x));
          glColor3f(color1->r, color1->g, color1->b);
        }
        glVertex3fv(p1);

        if(color){
          glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
          glNormal3fv(&(obj.verNormal[ obj.g[i].vertexIndices[j + 1]-1 ].x));
          glColor3f(color2->r, color2->g, color2->b);
        }
        glVertex3fv(p2);

        if(color){
          glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
          glNormal3fv(&(obj.verNormal[ obj.g[i].vertexIndices[j + 2]-1 ].x));
          glColor3f(color3->r, color3->g, color3->b);
        }
        glVertex3fv(p3);
      glEnd();

/*
      glBegin(GL_LINES);

      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 1]-1 ].x));

      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 1]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 2]-1 ].x));

      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j]-1 ].x));
      glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 2]-1 ].x));

      glEnd();
*/
    }
  }
}

void rotateByVec(objects* obj, glm::vec3 goalVec, char axis, glm::vec3 trans, bool updateData){

  glm::vec3 newX, newY, newZ, rotAxis;
  float rotAngle, dotVal;

  //goalVec is new z axis
  if(axis == 'z'){
    rotAxis = cross(obj->axis.z, goalVec);

    if(abs(rotAxis.x) < 1e-3 && abs(rotAxis.y) < 1e-3 && abs(rotAxis.z) < 1e-3){
      printf("no rotation\n");
      rotAxis = obj->axis.y;
      rotAngle = 0;
    }
    else{
      dotVal = dot(obj->axis.z, goalVec, true);
      if(dotVal > 1.0) dotVal = 1.0;
      else if(dotVal < -1.0) dotVal = -1.0;
      rotAngle = acos(dotVal);
    }
  }

  //goalVec is new y axis
  else if(axis == 'y'){
    rotAxis = cross(obj->axis.y, goalVec);
    if(abs(rotAxis.x) < 1e-3 && abs(rotAxis.y) < 1e-3 && abs(rotAxis.z) < 1e-3){
      printf("no rotation\n");
      rotAxis = obj->axis.z;
      rotAngle = 0;
    }
    else{
      dotVal = dot(obj->axis.y, goalVec, true);
      if(dotVal > 1.0) dotVal = 1.0;
      else if(dotVal < -1.0) dotVal = -1.0;
      rotAngle = acos(dotVal);
    }
  }

  //goalVec is new x axis
  else{
    rotAxis = cross(obj->axis.x, goalVec);
    if(abs(rotAxis.x) < 1e-3 && abs(rotAxis.y) < 1e-3 && abs(rotAxis.z) < 1e-3){
      printf("no rotation\n");
      rotAxis = obj->axis.y;
      rotAngle = 0;
    }
    else{
      dotVal = dot(obj->axis.x, goalVec, true);
      if(dotVal > 1.0) dotVal = 1.0;
      else if(dotVal < -1.0) dotVal = -1.0;
      rotAngle = acos(dotVal);
    }
  }

  float COS = cos(rotAngle);
  float SIN = sin(rotAngle);
  float a = rotAxis.x, b = rotAxis.y, c = rotAxis.z;
  float axisData[] = {
       (1 - a*a)*COS + a*a, -a*b*COS - c*SIN + a*b, -a*c*COS + b*SIN + a*c, 0,
    -a*b*COS + c*SIN + a*b,    (1 - b*b)*COS + b*b, -b*c*COS - a*SIN + b*c, 0,
    -a*c*COS - b*SIN + a*c, -b*c*COS + a*SIN + b*c,    (1 - c*c)*COS + c*c, 0,
                         0,                      0,                      0, 1};
  float dataX[] = {obj->axis.x.x, obj->axis.x.y, obj->axis.x.z, 1};
  float dataY[] = {obj->axis.y.x, obj->axis.y.y, obj->axis.y.z, 1};
  float dataZ[] = {obj->axis.z.x, obj->axis.z.y, obj->axis.z.z, 1};

  Mat axisRotMat = Mat(4, 4, CV_32F, axisData).clone();
  Mat axisX = Mat(4, 1, CV_32F, dataX).clone();
  Mat axisY = Mat(4, 1, CV_32F, dataY).clone();
  Mat axisZ = Mat(4, 1, CV_32F, dataZ).clone();

  axisX = axisRotMat * axisX;
  axisY = axisRotMat * axisY;
  axisZ = axisRotMat * axisZ;
  newX.x = axisX.at<float>(0, 0);
  newX.y = axisX.at<float>(1, 0);
  newX.z = axisX.at<float>(2, 0);
  newY.x = axisY.at<float>(0, 0);
  newY.y = axisY.at<float>(1, 0);
  newY.z = axisY.at<float>(2, 0);
  newZ.x = axisZ.at<float>(0, 0);
  newZ.y = axisZ.at<float>(1, 0);
  newZ.z = axisZ.at<float>(2, 0);

  //avoid not orthogonal
  newX = cross(newY, newZ);
  newY = cross(newZ, newX);

  if(updateData){
    float data[] = {
      newX.x, newY.x, newZ.x, trans.x,
      newX.y, newY.y, newZ.y, trans.y,
      newX.z, newY.z, newZ.z, trans.z,
           0,      0,      0,       1};
    Mat rotMat = Mat(4, 4, CV_32F, data).clone();
    float point[] = {0, 0, 0, 1.0};
    Mat oriPoint = Mat(4, 1, CV_32F, point).clone();
    Mat oriVec = oriPoint.clone();
    Mat newPoint, newVec;
    glm::vec3 newV;
    int len = obj->vertices.size();
    int normalLen = obj->normals.size();

    for(int i = 0; i < len; i++){

      oriPoint.at<float>(0, 0) = obj->vertices[i].x;
      oriPoint.at<float>(1, 0) = obj->vertices[i].y;
      oriPoint.at<float>(2, 0) = obj->vertices[i].z;

      newPoint = rotMat * oriPoint;

      obj->vertices[i].x = newPoint.at<float>(0, 0);
      obj->vertices[i].y = newPoint.at<float>(1, 0);
      obj->vertices[i].z = newPoint.at<float>(2, 0);

    }
    for(int i = 0; i < normalLen; i++){
      oriVec.at<float>(0, 0) = obj->normals[i].x;
      oriVec.at<float>(1, 0) = obj->normals[i].y;
      oriVec.at<float>(2, 0) = obj->normals[i].z;
      newVec = rotMat * oriVec;
      obj->normals[i].x = newVec.at<float>(0, 0);
      obj->normals[i].y = newVec.at<float>(1, 0);
      obj->normals[i].z = newVec.at<float>(2, 0);
    }
  }
  else{
    obj->axis.x = newX;
    obj->axis.y = newY;
    obj->axis.z = newZ;
    obj->position += trans;
  }
}

void rotation(objects* obj, float theta){
}

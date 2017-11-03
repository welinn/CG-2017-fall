//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
#include "object.h"

glm::vec3 cross(glm::vec3 v1, glm::vec3 v2){
  glm::vec3 n;
  n.x = v1.y * v2.z - v1.z * v2.y;
  n.y = v1.z * v2.x - v1.x * v2.z;
  n.z = v1.x * v2.y - v1.y * v2.x;
  float total = n.x*n.x + n.y*n.y + n.z*n.z;
  n /= total;
  return n;
}

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
  //printf("%d, %d\n", obj.vecColor.size(), obj.vertices.size());
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
      obj->g[i].normalIndices.push_back(len);
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

}

void drawObj(objects obj, bool color){

  int allObj = obj.g.size();
  vColor *color1, *color2, *color3;

  for(int i = 0; i < allObj; i++){
    int allMesh = obj.g[i].vertexIndices.size();
    for(int j = 0; j < allMesh; j += 3){
      color1 = &obj.verColor[obj.g[i].vertexIndices[j]-1];
      color2 = &obj.verColor[obj.g[i].vertexIndices[j + 1]-1];
      color3 = &obj.verColor[obj.g[i].vertexIndices[j + 2]-1];

      glBegin(GL_TRIANGLES);
        if(color){
          glNormal3fv(&(obj.verNormal[ obj.g[i].vertexIndices[j]-1 ].x));
          glColor3f(color1->r, color1->g, color1->b);
        }
        glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j]-1 ].x));

        if(color){
          glNormal3fv(&(obj.verNormal[ obj.g[i].vertexIndices[j + 1]-1 ].x));
          glColor3f(color2->r, color2->g, color2->b);
        }
        glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 1]-1 ].x));

        if(color){
          glNormal3fv(&(obj.verNormal[ obj.g[i].vertexIndices[j + 2]-1 ].x));
          glColor3f(color3->r, color3->g, color3->b);
        }
        glVertex3fv(&(obj.vertices[ obj.g[i].vertexIndices[j + 2]-1 ].x));
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



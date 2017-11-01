//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
#include "loadObj.h"

objects getObjInfo(char* path){

  FILE* file = fopen(path, "r");
  if( file == NULL ){
    printf("Open file fault\n");
    exit(EXIT_FAILURE);
  }

  objMeshs meshs;
  objects obj;
  char input[128];
  bool first = true;
  bool newMeshs = false;

  while(fscanf(file, "%s", input) != EOF){
    //vertex
    if(strcmp(input, "v") == 0){

      glm::vec3 vertex;
      fscanf(file, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
      obj.vertices.push_back(vertex);

      char str[300];
      char *ptr = str;
      vColor vc;

      fgets(str, 300, file);
      while(*ptr == ' ') ptr++;
      if(*ptr <= '9' && *ptr >= '0'){
        int len = strlen(str);
        fseek(file, -len, SEEK_CUR);
        fscanf(file, "%f %f %f\n", &vc.r, &vc.g, &vc.b);
        obj.vecColor.push_back(vc);
      }
      else{
        vc.r = -1;
        vc.g = -1;
        vc.b = -1;
      }
      obj.vecColor.push_back(vc);

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
      char str[300], in[300];
      int len, length, element;

      fgets(in, 300, file);
      strcpy(str, in);
      length = strlen(str) - 1;  //the return key(enter(13)) included in length
      char *tmp = strtok(str, " /");

      element = 0;
      len = 1;  //the space after 'f'
      while(tmp != NULL){
        value = atoi(tmp);
        len += strlen(tmp);

        if(*(in + len) == ' ' || len == length){

          if(element == 0){
            meshs.vertexIndices.push_back(value);
            meshs.uvIndices.push_back(-1);
            meshs.normalIndices.push_back(-1);

          }
          else if(element == 1){
            meshs.uvIndices.push_back(value);
            meshs.normalIndices.push_back(-1);
          }
          else if(element == 2){
            meshs.normalIndices.push_back(value);
          }
          element = 0;
        }
        else if(*(in + len) == '/'){

          bool noNum = false;

          if(element == 0){
            meshs.vertexIndices.push_back(value);
            if(*(in + len + 1) ==  '/'){
              meshs.uvIndices.push_back(-1);
              element++;
              if(*(in + len + 2) ==  ' '){
                meshs.normalIndices.push_back(-1);
                element++;
              }
            }
          }
          else if(element == 1){
            meshs.uvIndices.push_back(value);
            if(*(in + len + 1) ==  ' ') meshs.normalIndices.push_back(-1);
            element++;
          }
          else if(element == 2){
            meshs.normalIndices.push_back(value);

          }
          element++;
        }
        tmp = strtok(NULL, " /");
        len++;
      }

      if(newMeshs){
        objMeshs meshs;
        newMeshs = false;
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

  return obj;
}

//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
#include "loadObj.h"

void objMeshInit(objMeshs *meshs){
  meshs->vi = 0;
  meshs->uvi = 0;
  meshs->ni = 0;
}

objects getObjInfo(char* path){

  FILE* file = fopen(path, "r");
  if( file == NULL ){
    printf("Impossible to open the file !\n");
    exit(EXIT_FAILURE);
  }

  objMeshs *meshs;
  objects obj;
  char input[128];
  bool first = true, vOnly = true, checked = false;

  meshs = (objMeshs*) malloc(sizeof(objMeshs));
  objMeshInit(meshs);

  while(fscanf(file, "%s", input) != EOF){
    //vertex
    if(strcmp(input, "v") == 0){
      glm::vec3 vertex;
      fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
      //vertex *= 10;
      obj.vertices.push_back(vertex);
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

      if(!checked){  //check "/"
        checked = true;
        char str[50];
        fscanf(file, "%s", str);
        char *test = strtok(str, "/");
        test = strtok(NULL, "/");
        if(test != NULL) vOnly = false;
        else vOnly = true;

        int len = strlen(str);
        fseek(file, -len, SEEK_CUR);
      }

      unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
      if(vOnly){
        int matches = fscanf(file, "%u %u %u\n",
            &vertexIndex[0], &vertexIndex[1], &vertexIndex[2] );
      }
      else{
        int matches = fscanf(file, "%u/%u/%u %u/%u/%u %u/%u/%u\n",
            &vertexIndex[0], &uvIndex[0], &normalIndex[0],
            &vertexIndex[1], &uvIndex[1], &normalIndex[1],
            &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
        if (matches != 9){
          printf("Read mesh fail\n");
          exit(EXIT_FAILURE);
        }
        meshs -> uvIndices    [meshs->uvi++] = uvIndex[0];
        meshs -> uvIndices    [meshs->uvi++] = uvIndex[1];
        meshs -> uvIndices    [meshs->uvi++] = uvIndex[2];
        meshs -> normalIndices[meshs->ni++] = normalIndex[0];
        meshs -> normalIndices[meshs->ni++] = normalIndex[1];
        meshs -> normalIndices[meshs->ni++] = normalIndex[2];
      }
      meshs -> vertexIndices[meshs->vi++] = vertexIndex[0];
      meshs -> vertexIndices[meshs->vi++] = vertexIndex[1];
      meshs -> vertexIndices[meshs->vi++] = vertexIndex[2];
    }
    //different obj
    else if(strcmp(input, "g") == 0){
      if(first) first = false;
      else{
        obj.g.push_back(*meshs);
        meshs = (objMeshs*) malloc(sizeof(objMeshs));
        objMeshInit(meshs);
      }
    }
  }
  obj.g.push_back(*meshs);

  return obj;
}

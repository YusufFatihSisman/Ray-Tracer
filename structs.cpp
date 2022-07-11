#include "structs.h"
#include <vector>
#include <iostream>

using namespace std;

void initMaterial(Material& material, int id, double* ambient, double* diffuse, double* specular, double phongexponent, double* reflectance){
    int i = 0;
    material.id = id;
    material.phongexponent = phongexponent;
    
    material.ambient = color(ambient[0], ambient[1], ambient[2]);
    material.diffuse = color(diffuse[0], diffuse[1], diffuse[2]);
    material.specular = color(specular[0], specular[1], specular[2]);
    material.reflectance = color(reflectance[0], reflectance[1], reflectance[2]);
    
}

void initCamera(Camera& camera, double* position, double* gaze, double* up, int* nearplane, double neardistance, int* imageresolution){
    int i = 0;
    camera.neardistance = neardistance;
    camera.position= point3(position[0], position[1], position[2]);
    camera.gaze = vec3(gaze[0], gaze[1], gaze[2]);
    camera.up = vec3(up[0], up[1], up[2]); 

    for(i = 0; i < 4; i++){
        camera.nearplane[i] = nearplane[i];
        if(i < 2){
            camera.imageresolution[i] = imageresolution[i];
        }
    }
}

void initMesh(Mesh& mesh, int id, int materialId, vector<int> faces){
    int i = 0;
    mesh.id = id;
    mesh.materialId = materialId;
    mesh.faces;
    for(i = 0; i < faces.size(); i++){
        mesh.faces.push_back(faces[i]);
    }
}
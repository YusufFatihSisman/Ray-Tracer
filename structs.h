#ifndef STRUCTS_H
#define STRUCTS_H
#include <vector>
#include "vec3.h"
#include <memory>
#include <limits>

using std::vector;
using std::shared_ptr;
using std::make_shared;

const double EPS = 0.0001;
const double INFINITY_T = std::numeric_limits<double>::infinity();

struct Material
{
    int id;
    color ambient;
    color diffuse;
    color specular;
    double phongexponent;
    color reflectance;
};

void initMaterial(Material& material, int id, double* ambient, double* diffuse, double* specular, double phongexponent, double* reflectance);

struct Camera
{
    point3 position;
    vec3 gaze;
    vec3 up;
    int nearplane[4];
    double neardistance;
    int imageresolution[2];    
};

void initCamera(Camera& camera, double* position, double* gaze, double* up, int* nearplane, double neardistance, int* imageresolution);

struct Mesh
{
    int id;
    int materialId;
    vector<int> faces;
};

void initMesh(Mesh& mesh, int id, int materialId, vector<int> faces);


struct hit_record {
    point3 p;
    vec3 normal;
    double t;
    shared_ptr<Material> mat_ptr;
};
#endif
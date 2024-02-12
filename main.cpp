#include <iostream>
#include <fstream>
#include <vector>
#include "rapidxml-1.13/rapidxml.hpp"
#include "helper.h"
#include "structs.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "face.h"
#include "faceList.h"
#include "pointLight.h"
#include "bvh.h"
#include <thread>
#include "time.h"
//#include <bits/stdc++.h>

using namespace std;
using namespace rapidxml;

int maxraytracedepth = -1;
color background;
color ambientlight;
vector<double> vertexdata;
vector<Mesh> meshes;
vector<shared_ptr<Material>> materials;
vector<pointLight> pointLights;
thread threads[8];
Camera camera;
faceList faces = faceList();
vector<bvh> boundIngVolumeHierarchies;
bvh finalBvh;

void parse_xml(string filename){
    xml_document<> doc;
    xml_node<> * root_node = NULL;

    ifstream theFile (filename);
	if(!theFile)
		return;
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');

    doc.parse<0>(&buffer[0]);
   
    root_node = doc.first_node("scene");

    string data = root_node->first_node("maxraytracedepth")->value();
    maxraytracedepth = atoi(data.c_str());

    data =  root_node->first_node("background")->value();
    double bg[3];
    parse_string_and_assign<double>(data, bg);
    background = color(bg[0], bg[1], bg[2]);
    
    double position[3];
    double gaze[3];
    double up[3];
    int nearplane[4];
    double neardistance;
    int imageresolution[2]; 

    xml_node<>* cameraNode =  root_node->first_node("camera");
    data = cameraNode->first_node("position")->value();
    parse_string_and_assign<double>(data, position);   
    data = cameraNode->first_node("gaze")->value();
    parse_string_and_assign<double>(data, gaze);
    data = cameraNode->first_node("up")->value();
    parse_string_and_assign<double>(data, up);
    data = cameraNode->first_node("nearplane")->value();
    parse_string_and_assign<int>(data, nearplane);
    data = cameraNode->first_node("neardistance")->value();
    neardistance = atof(data.c_str());
    data = cameraNode->first_node("imageresolution")->value();
    parse_string_and_assign<int>(data, imageresolution);
    
    initCamera(camera, position, gaze, up, nearplane, neardistance, imageresolution);
    xml_node<>* light_node =  root_node->first_node("lights");

    data =  light_node->first_node("ambientlight")->value();
    double amb[3];
    parse_string_and_assign<double>(data, amb);
    ambientlight = color(amb[0], amb[1], amb[2]);

    //point lights
    for (xml_node<> * point_light_node = light_node->first_node("pointlight"); point_light_node; point_light_node = point_light_node->next_sibling())
    {
        data = point_light_node->first_attribute("id")->value();
        int id = atoi(data.c_str());

        data = point_light_node->first_node("position")->value();
        parse_string_and_assign<double>(data, position);

        double intensity[3];
        data = point_light_node->first_node("intensity")->value(); 
        parse_string_and_assign<double>(data, intensity);

        pointLight pLight = pointLight(id, point3(position[0], position[1], position[2]), color(intensity[0], intensity[1], intensity[2]));
        pointLights.push_back(pLight);
    }

    //materials
    xml_node<>* materials_node =  root_node->first_node("materials");
    for (xml_node<> * material_node = materials_node->first_node("material"); material_node; material_node = material_node->next_sibling())
    {
        data = material_node->first_attribute("id")->value();
        int id = atoi(data.c_str());

        double ambient[3];
        data = material_node->first_node("ambient")->value();
        parse_string_and_assign<double>(data, ambient);

        double diffuse[3];
        data = material_node->first_node("diffuse")->value();
        parse_string_and_assign<double>(data, diffuse);
    
        double specular[3];
        data = material_node->first_node("specular")->value();
        parse_string_and_assign<double>(data, specular);

        data = material_node->first_node("phongexponent")->value();
        double phongexponent = atof(data.c_str());

        double reflectance[3];
        data = material_node->first_node("mirrorreflectance")->value();
        parse_string_and_assign<double>(data, reflectance);

        Material material;
        initMaterial(material, id, ambient, diffuse, specular, phongexponent, reflectance);
        shared_ptr<Material> mt(new Material);
        *mt = material;
        materials.push_back(mt);

        theFile.close();
    }

    data = root_node->first_node("vertexdata")->value();
    parse_string_and_assign_vector(data, vertexdata);

   //objects
    xml_node<>* objects_node =  root_node->first_node("objects");
    for (xml_node<> * mesh_node = objects_node->first_node("mesh"); mesh_node; mesh_node = mesh_node->next_sibling())
    {
        
        data = mesh_node->first_attribute("id")->value();
        int id = atoi(data.c_str());
        
        data = mesh_node->first_node("materialid")->value();

        int materialId = atoi(data.c_str());
        
        vector<int> faces;
        data = mesh_node->first_node("faces")->value();
        parse_string_and_assign_vector<int>(data, faces);

        Mesh mesh;
        initMesh(mesh, id, materialId, faces);
        meshes.push_back(mesh);
    }

}       

color ray_color(const ray& r, const bvh& boundingVH, int depth) {
    hit_record hr;             
    color c = color(0.0, 0.0, 0.0);
    if(boundingVH.hit(r, 0.0, INFINITY_T, hr)){ 
        for(const pointLight& light : pointLights){
            if(depth > 0 && (hr.mat_ptr->reflectance.x == 1.0 || hr.mat_ptr->reflectance.y == 1.0 || hr.mat_ptr->reflectance.z == 1.0)){
                vec3 w0 = unit_vector(r.origin() - hr.p);
                vec3 wr = -w0 + 2*hr.normal*(dot(hr.normal, w0));
                c += light.illuminate(r, hr, boundingVH) + hr.mat_ptr->reflectance * ray_color(ray((hr.p + wr * EPS), wr), boundingVH, depth-1);
            }else{
                c += light.illuminate(r, hr, boundingVH);  
            }
        }           
        return c + hr.mat_ptr->ambient * ambientlight; 
    }                   
    return background;          
}

void setData(){     
    for(int i=0; i < meshes.size(); i++){
        int j = 0;
        // create facelist hear and reset
        faceList faces = faceList();
        while(j < meshes[i].faces.size()){
            int index = (meshes[i].faces[j]-1)*3;
            point3 p0 = point3(vertexdata[index], vertexdata[index+1], vertexdata[index+2]);
            index =  (meshes[i].faces[j+1]-1)*3;
            point3 p1 = point3(vertexdata[index], vertexdata[index+1], vertexdata[index+2]);
            index =  (meshes[i].faces[j+2]-1)*3;
            point3 p2 = point3(vertexdata[index], vertexdata[index+1], vertexdata[index+2]);
            for(int k = 0; k < materials.size(); k++){
                if(meshes[i].materialId == materials[k]->id){
                    shared_ptr<face> f(new face(p0, p1, p2, materials[k])); 
                    faces.add(f); 
                }   
            }   
            j += 3;
        }
        // sort facelist and create bvh
        faces.sort();
        /*for(const auto& object : faces->objects){
            double thisOrigin = (object->vertices[0].x + object->vertices[1].x + object->vertices[2].x) / 3;
            cout << thisOrigin << "\n";
        } */
        bvh boundingVolumeHierarchy = bvh(faces.objects);
        boundIngVolumeHierarchies.push_back(boundingVolumeHierarchy);
        //boundingVolumeHierarchy.printBT();
        //cout << "\n\n";
        //delete(faces);
    } 

    for (int i = 0; i < boundIngVolumeHierarchies.size() - 1; i++) { 
        int max_idx = i; 
        for (int j = i + 1; j < boundIngVolumeHierarchies.size() ; j++) 
        {
            if (boundIngVolumeHierarchies[j] < boundIngVolumeHierarchies[max_idx]) 
                max_idx = j; 
        }
        bvh temp =  boundIngVolumeHierarchies[max_idx];
        boundIngVolumeHierarchies[max_idx] = boundIngVolumeHierarchies[i];
        boundIngVolumeHierarchies[i] = temp;
    } 

    finalBvh = vectorToBvh(boundIngVolumeHierarchies);
    //finalBvh.printBT();
    /*for(const auto& object : faces.objects){
        cout << object->mat_ptr.use_count() << "\n";
    }  */  

}

void calc_values(color** array, int i_start, int j_start, int i_end, int j_end, 
int nx, int ny, point3 origin, double l, double r, double b, double t, vec3 u, vec3 v, point3 m, point3 q){
    for(int j = j_start; j < j_end; j++){  
        for(int i = i_start; i < i_end; i++){
            double su = (r-l)*(i + 0.5)/nx;  
            double sv = (t-b)*(j + 0.5)/ny;
            point3 s = q + su*u - sv*v;
            vec3 dir = s - origin;   
            ray r(origin, dir); 
            color pixel_color = ray_color(r, finalBvh, maxraytracedepth);
            array[j][i] = normalizeColor(pixel_color);     
        }
    }
}

int main(int argc, char** argv){
	if(argc == 1)
		return 0;
    time_t start, end;
    time(&start);
	
    parse_xml(argv[1]);
	if(maxraytracedepth == -1)
		return 0;
	
    setData();   
    
    int nx = camera.imageresolution[0];
    int ny = camera.imageresolution[1];
    color** array = new color*[ny];
    for(int i = 0; i < ny; ++i)
        array[i] = new color[nx];

    point3 origin = point3(camera.position.x, camera.position.y, camera.position.z);
    double l = camera.nearplane[0];
    double r = camera.nearplane[1]; 
    double b = camera.nearplane[2]; 
    double t = camera.nearplane[3];
    double dist = camera.neardistance;
    vec3 w = -vec3(camera.gaze.x, camera.gaze.y, camera.gaze.z);
    vec3 v = vec3(camera.up.x, camera.up.y, camera.up.z);
    vec3 u = cross(v, w);       
    point3 m = origin + -w*dist;   
    point3 q = m + l*u + t*v;

    int yPerThread = ny / 8;
    int x = 0;
    int y = 0;
    for(int j = 0; j < 7; j++){
        int yEnd = y + yPerThread;
        threads[j] = thread(calc_values, array, x, y, nx, yEnd, nx, ny, origin, l, r, b, t, u, v, m, q);
        y = yEnd;    
    }    
    threads[7] = thread(calc_values, array, x, y, nx, ny, nx, ny, origin, l, r, b, t, u, v, m, q); 
        
    for(int j = 0; j < 8; j++){
		threads[j].join();
	}
	time(&end);
	double time_taken = double(end - start);
    std::cerr << "Time taken before print : " << fixed
         << time_taken ; //<< setprecision(5);
    std::cerr << " sec " << endl;
	
	std::cout << "P3\n" << nx << ' ' << ny << "\n255\n";
    for(int j = 0; j < ny; j++){    
        //std::cerr << "\rLine Completed: " << j << ' ' << std::flush;
		for (int i = 0; i < nx; i++) {
			std::cout << array[j][i] << "\n";
		}
	}
	
    for(int i = 0; i < ny; i++) {
        delete[] array[i];   
    }
    delete[] array;

    time(&end);
    time_taken = double(end - start);
    std::cerr << "Time taken by program is : " << fixed
         << time_taken ; //<< setprecision(5);
    std::cerr << " sec " << endl;
    
    return 0;
}

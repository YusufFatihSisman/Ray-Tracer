#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "ray.h"
#include "structs.h"
#include "vec3.h"
#include "bvh.h"
#include <math.h> 

using std::max;

class pointLight{
    public:
        pointLight(){}
        pointLight(int id,  point3 pos, color intensity){
            this->id = id;
            i = intensity;
            p = pos;
        }

        color illuminate(const ray& r, const hit_record& hr, const bvh& bvh) const;

        int id;
        color i;
        point3 p;
};


color pointLight::illuminate(const ray& r, const hit_record& hr, const bvh& bvh) const{
    point3 x = hr.p;
    vec3 wi = (p - x);
    double len = wi.length();
    vec3 l = (p - x) / len;
    ray newRay = ray((x + l * EPS), l);
    hit_record shadow;
    /*for(const auto& face : faces.objects){
        if(face->hit(newRay, 0, len, shadow)){
            return color(0.0, 0.0, 0.0);
        }
    }*/
    if(bvh.hit(newRay, 0, len, shadow))
    {
        return color(0.0, 0.0, 0.0);
    }
    vec3 w0 = r.origin() - x;
    vec3 h = (wi + w0) / (wi + w0).length();
    vec3 n = hr.normal;
    color irradiance = i/(len*len);
    color specular = pow(max(0.0, dot(n, h)), hr.mat_ptr->phongexponent)*irradiance;
    color diffuse = max(0.0, dot(n, l))*irradiance;
    color k = hr.mat_ptr->diffuse; 
    color s = hr.mat_ptr->specular; 
    return k*diffuse + s*specular;
}

#endif
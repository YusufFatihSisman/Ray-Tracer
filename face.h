#ifndef FACE_H
#define FACE_H

#include "vec3.h"
#include "ray.h"
#include "structs.h"

class face{
    public:
        face(point3 p0, point3 p1, point3 p2): vertices{p0, p1, p2} {}
        face(point3 p0, point3 p1, point3 p2, shared_ptr<Material> object) : vertices{p0, p1, p2} {
            mat_ptr = object;
        }
        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

        point3 vertices[3];
        shared_ptr<Material> mat_ptr; 
};

bool face::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    double a = vertices[0].x - vertices[1].x;
    double b = vertices[0].y - vertices[1].y;
    double c = vertices[0].z - vertices[1].z;

    double d = vertices[0].x - vertices[2].x;
    double e = vertices[0].y - vertices[2].y;
    double f = vertices[0].z - vertices[2].z;

    double j = vertices[0].x - r.origin().x;
    double k = vertices[0].y - r.origin().y;
    double l = vertices[0].z - r.origin().z;

    double ei = e * r.direction().z;
    double hf = f * r.direction().y;
    double gf = f * r.direction().x;
    double di = d * r.direction().z;
    double dh = d * r.direction().y;
    double eg = e * r.direction().x;

    double ak = a * k;
    double jb = j * b;
    double jc = j * c;
    double al = a * l;
    double bl = b * l;
    double kc = k * c;

    double M = a*(ei - hf) + b*(gf - di) + c*(dh-eg); 

    double t = -(f*(ak - jb) + e*(jc - al) + d*(bl - kc))/M;
    if(t < t_min || t > t_max)
        return false;
    
    double gamma = (r.direction().z*(ak - jb) + r.direction().y*(jc - al) + r.direction().x*(bl - kc))/M;
    if(gamma < 0 || gamma > 1)
        return false;

    double beta = (j*(ei - hf) + k*(gf - di) + l*(dh - eg))/M;
    if(beta < 0 || beta > 1 - gamma){
        return false;
    }
            
    rec.t = t;
    rec.p = r.at(rec.t);
    rec.normal = unit_vector(cross((vertices[1] - vertices[0]), (vertices[2] - vertices[0])));
    rec.mat_ptr = mat_ptr;
    return true;
}

#endif
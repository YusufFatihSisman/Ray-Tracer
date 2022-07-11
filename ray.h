#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray{
    public:
        ray(const point3& origin, const vec3& direction): org(origin), dir(direction){}

        point3 at(double t) const{
            return org + t*dir;
        }

        point3 origin() const  { return org; }
        vec3 direction() const { return dir; }
        
    private:
        point3 org;
        vec3 dir;
};

#endif
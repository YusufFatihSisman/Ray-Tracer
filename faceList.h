#ifndef FACELIST_H
#define FACELIST_H

#include "face.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class faceList{
    public:
        faceList(){}
        faceList(shared_ptr<face> object){add(object);}

        void clear() { objects.clear(); }
        void add(shared_ptr<face> object) { objects.push_back(object); }

        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

        std::vector<shared_ptr<face>> objects;
};

bool faceList::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    hit_record temp;
    bool hit = false;
    double closest_t = t_max;

    for(const auto& object : objects){
        if(object->hit(r, t_min, closest_t, temp)){
            hit = true;
            rec = temp;
            closest_t = temp.t;
        }
    }
    return hit;
}

#endif
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
        faceList(vector<shared_ptr<face>> faces) {objects = faces;}

        void clear() { objects.clear(); }
        void add(shared_ptr<face> object) { objects.push_back(object); }

        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

        void sort();

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

void faceList::sort(){
    for (int i = 0; i < objects.size() - 1; i++) { 
        int max_idx = i; 
        for (int j = i + 1; j < objects.size() ; j++) 
        {
            if (*objects[j] < *objects[max_idx]) 
                max_idx = j; 
        }
        objects[max_idx].swap(objects[i]);
    }
}

#endif
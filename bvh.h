#ifndef BVH_H
#define BVH_H

#include "faceList.h"
#include "vec3.h"
#include <vector>
#include <iterator>
#include <memory>

using std::shared_ptr;

class bvh{
    faceList fclst;
    bvh* left;
    bvh* right;
    point3 leftFrontBottom;
    point3 rightBackTop;

    public:
        bvh(){}
        bvh(const bvh& leftBvh, const bvh& rightBvh){
            fclst = faceList();
            left = new bvh(leftBvh);
            right = new bvh(rightBvh);

            double minX = leftBvh.leftFrontBottom.x < rightBvh.leftFrontBottom.x ? leftBvh.leftFrontBottom.x : rightBvh.leftFrontBottom.x;
            double minY = leftBvh.leftFrontBottom.y < rightBvh.leftFrontBottom.y ? leftBvh.leftFrontBottom.y : rightBvh.leftFrontBottom.y;
            double minZ = leftBvh.leftFrontBottom.z < rightBvh.leftFrontBottom.z ? leftBvh.leftFrontBottom.z : rightBvh.leftFrontBottom.z;
            leftFrontBottom = point3(minX, minY, minZ);

            
            double maxX = leftBvh.rightBackTop.x > rightBvh.rightBackTop.x ? leftBvh.rightBackTop.x : rightBvh.rightBackTop.x;
            double maxY = leftBvh.rightBackTop.y > rightBvh.rightBackTop.y ? leftBvh.rightBackTop.y : rightBvh.rightBackTop.y;
            double maxZ = leftBvh.rightBackTop.z > rightBvh.rightBackTop.z ? leftBvh.rightBackTop.z : rightBvh.rightBackTop.z;
            rightBackTop = point3(maxX, maxY, maxZ);
        }

        bvh(const bvh &copyBvh){
            fclst = faceList(copyBvh.fclst.objects);
            left = copyBvh.left;
            right = copyBvh.right;
            leftFrontBottom = copyBvh.leftFrontBottom;
            rightBackTop = copyBvh.rightBackTop;
        }

        bvh(const vector<shared_ptr<face>> &faces){
            if(faces.size() > 10)
            {
                int half_size = faces.size() / 2;
                vector<shared_ptr<face>> split_lo(faces.begin(), faces.begin() + half_size);
                vector<shared_ptr<face>> split_hi(faces.begin() + half_size, faces.end());
                
                findBoxBorders(faces);
                fclst = faceList();
                left = new bvh(split_lo);
                right = new bvh(split_hi);
                return;
            }

            findBoxBorders(faces);
            fclst = faceList(faces);
            left = NULL;
            right = NULL;
        }

        bool operator<(const bvh& sbvh);

        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

        void printBT(const std::string& prefix, const bvh* node, bool isLeft);

        void printBT(){ printBT("", this, false); }

    private:
        bool hit(const ray& r, double t_min, double t_max) const;
        //double& x1, double& y1, double& z1, double& x2, double& y2, double& z2
        void findBoxBorders(const vector<shared_ptr<face>> &faces);
};

bool bvh::hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    if(this->hit(r, t_min, t_max) == false)
    {
        return false;
    } 
    
    if(fclst.objects.size() != 0)
    {
        return fclst.hit(r, t_min, t_max, rec);
    }
    rec.t = INFINITY_T;
    hit_record rec1, rec2;
    bool hitLeft = left->hit(r, t_min, t_max, rec1);
    bool hitRight = right->hit(r, t_min, t_max, rec2);

    if(hitLeft)
    {
        rec = rec1;
    }
    if(hitRight)
    {
        rec = rec2.t < rec.t ? rec2 : rec;
    }

    return(hitLeft || hitRight);

}

bool bvh::hit(const ray& r, double t_min, double t_max) const{
    if(r.direction().x == 0 && (r.origin().x < leftFrontBottom.x || r.origin().x > rightBackTop.x))
        return false;
    if(r.direction().y == 0 && (r.origin().y < leftFrontBottom.y || r.origin().y > rightBackTop.y))
        return false;
    if(r.direction().z == 0 && (r.origin().z < leftFrontBottom.z || r.origin().z > rightBackTop.z))
        return false;
    
    double oneOverRdx = 1.0/r.direction().x;
    double oneOverRdy = 1.0/r.direction().y;
    double oneOverRdz = 1.0/r.direction().z;

    double t1 = (leftFrontBottom.x - r.origin().x) * oneOverRdx;
    double t2 = (rightBackTop.x - r.origin().x) * oneOverRdx;
    if(t1 > t2){
        double temp = t2;
        t2 = t1;
        t1 = temp;
    }   
    double tStart = t1;
    double tEnd = t2;

    t1 = (leftFrontBottom.y - r.origin().y) * oneOverRdy;
    t2 = (rightBackTop.y - r.origin().y) * oneOverRdy;
    if(t1 > t2){
        double temp = t2;
        t2 = t1;
        t1 = temp;
    }   
    if(t1 > tStart){
        tStart = t1;
    } 
    if(t2 < tEnd){
        tEnd = t2;
    }
    
    t1 = (leftFrontBottom.z - r.origin().z) * oneOverRdz;
    t2 = (rightBackTop.z - r.origin().z) * oneOverRdz;
    if(t1 > t2){
        double temp = t2;
        t2 = t1;
        t1 = temp;
    }   
    if(t1 > tStart){
        tStart = t1;
    } 
    if(t2 < tEnd){
        tEnd = t2;
    }
        

    if(tStart > tEnd){
        return false;
    }

    //cout << "first condition passed\n";

    if(tEnd < t_min){
        return false;
    }

    //cout << "second condition passed\n";
    
    if(tStart > t_min){
        return true;
    }

    return true;
}

bool bvh::operator<(const bvh& sbvh){
    double thisOriginx = (leftFrontBottom.x + rightBackTop.x) / 2;
    //double thisOriginy = (leftFrontBottom.y + rightBackTop.y) / 2;
    //double thisOriginz = (leftFrontBottom.z + rightBackTop.z) / 2;
    //point3 thisOrigin = point3(thisOriginx, thisOriginy, thisOriginz);

    double otherOriginx = (sbvh.leftFrontBottom.x + sbvh.rightBackTop.x) / 2;
    //double otherOriginy = (sbvh.leftFrontBottom.y + sbvh.rightBackTop.y) / 2;
    //double otherOriginz = (sbvh.leftFrontBottom.z + sbvh.rightBackTop.z) / 2;
    
    return thisOriginx < otherOriginx;
}

void bvh::printBT(const std::string& prefix, const bvh* node, bool isLeft){
    if( node != nullptr )
    {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        //std::cout << node->fclst.objects.size() << std::endl;
        std::cout << node->leftFrontBottom << "   " << node->rightBackTop << "   " << node->fclst.objects.size() << std::endl;

        // enter the next tree level - left and right branch
        printBT( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBT( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

void bvh::findBoxBorders(const vector<shared_ptr<face>> &faces){
    double x1 = faces[0]->min(0);
    double y1 = faces[0]->min(1);
    double z1 = faces[0]->min(2);

    double x2 = faces[0]->max(0);
    double y2 = faces[0]->max(1);
    double z2 = faces[0]->max(2);

    for(int i = 1; i < faces.size(); i++){
        if(x1 > faces[i]->min(0))
            x1 = faces[i]->min(0);
        if(y1 > faces[i]->min(1))
            y1 = faces[i]->min(1);
        if(z1 > faces[i]->min(2))
            z1 = faces[i]->min(2);

        if(x2 < faces[i]->max(0))
            x2 = faces[i]->max(0);
        if(y2 < faces[i]->max(1))
            y2 = faces[i]->max(1);
        if(z2 < faces[i]->max(2))
            z2 = faces[i]->max(2);
    }

    leftFrontBottom = point3(x1, y1, z1);
    rightBackTop = point3(x2, y2, z2);
}

bvh vectorToBvh(vector<bvh> &bvhVector){
    //bvh root;

    if(bvhVector.size() == 1){
        return bvhVector[0];
    }

    bool first = true;
    vector<bvh>::iterator it = bvhVector.begin();

    while(bvhVector.size() >= 2)
    {
        if(first)
        {
            it = bvhVector.begin();
            bvh root = bvh(bvhVector[0], bvhVector[1]);
            bvhVector.erase(it);
            it = bvhVector.begin();
            bvhVector.erase(it);
            it = bvhVector.begin();
            bvhVector.insert(it, root);
            first = false;
        }
        else
        {
            it = bvhVector.end();
            bvh root = bvh(bvhVector[bvhVector.size()-2], bvhVector[bvhVector.size()-1]);
            bvhVector.erase(it);
            it = bvhVector.end();
            bvhVector.erase(it);
            it = bvhVector.begin();
            bvhVector.insert(it, root);
            first = true;
        }
    }

    return bvhVector[0];

}
#endif
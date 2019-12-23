#pragma once

#include "ofMain.h"
#include "ofxOctreeNode.h"
#include <unordered_set>

template<class N>
class ofxOctree {
    public:
        ofxOctree();
        ofxOctree(const glm::vec3& _bbMin, const glm::vec3& _bbMax, int _maxItems = 1, float _minSize = 0.1);
        void addItem(shared_ptr<N> _item);
        vector<shared_ptr<N>> queryBox(const glm::vec3& _bbMin, const glm::vec3& _bbMax);
        vector<shared_ptr<N>> queryRay(const glm::vec3& _origin, const glm::vec3& _direction);
        vector<shared_ptr<N>> queryPlane(const glm::vec3& _normal, const float& _distance);
        vector<shared_ptr<N>> getMembers();
        void clear();
    private:
        glm::vec3 minVec(const glm::vec3& _v1, const glm::vec3& _v2);
        glm::vec3 maxVec(const glm::vec3& _v1, const glm::vec3& _v2);
        vector<shared_ptr<N>> members;
        shared_ptr<ofxOctreeNode<N>> top;
};

template<class N>
ofxOctree<N>::ofxOctree() {
    top = make_shared<ofxOctreeNode<N>>(ofxOctreeNode<N>(glm::vec3(0), glm::vec3(0), 1, 0.1));
}

template<class N>
ofxOctree<N>::ofxOctree(const glm::vec3& _bbMin, const glm::vec3& _bbMax, int _maxItems, float _minSize) {
    top = make_shared<ofxOctreeNode<N>>(ofxOctreeNode<N>(minVec(_bbMin, _bbMax), maxVec(_bbMin, _bbMax), _maxItems, _minSize));
}

template<class N>
void ofxOctree<N>::addItem(shared_ptr<N> _item) {
    members.push_back(_item);
    top->addItem(_item);
}

template<class N>
vector<shared_ptr<N>> ofxOctree<N>::queryBox(const glm::vec3& _bbMin, const glm::vec3& _bbMax) {
    unordered_set<shared_ptr<N>> result;
    top->queryBox(minVec(_bbMin, _bbMax), maxVec(_bbMin, _bbMax), result);
    return vector<shared_ptr<N>>(result.begin(), result.end());
}

template<class N>
vector<shared_ptr<N>> ofxOctree<N>::queryRay(const glm::vec3& _origin, const glm::vec3& _direction) {
    unordered_set<shared_ptr<N>> result;
    top->queryRay(_origin, _direction, result);
    return vector<shared_ptr<N>>(result.begin(), result.end());
}

template<class N>
vector<shared_ptr<N>> ofxOctree<N>::queryPlane(const glm::vec3& _normal, const float& _distance) {
    unordered_set<shared_ptr<N>> result;
    top->queryPlane(_normal, _distance, result);
    return vector<shared_ptr<N>>(result.begin(), result.end());
}

template<class N>
vector<shared_ptr<N>> ofxOctree<N>::getMembers() {
    return members;
}

template<class N>
void ofxOctree<N>::clear() {
    delete top;
    top = nullptr;
}

template<class N>
glm::vec3 ofxOctree<N>::minVec(const glm::vec3& _v1, const glm::vec3& _v2) {
    return glm::vec3(MIN(_v1.x, _v2.x), MIN(_v1.y, _v2.y), MIN(_v1.z, _v2.z));
}

template<class N>
glm::vec3 ofxOctree<N>::maxVec(const glm::vec3& _v1, const glm::vec3& _v2) {
    return glm::vec3(MAX(_v1.x, _v2.x), MAX(_v1.y, _v2.y), MAX(_v1.z, _v2.z));
}

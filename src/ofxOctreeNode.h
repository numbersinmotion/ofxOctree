#pragma once

#include "ofMain.h"
#include <unordered_set>

template<class N>
class ofxOctreeNode {
    public:
        ofxOctreeNode();
        ofxOctreeNode(const glm::vec3& _bbMin, const glm::vec3& _bbMax, int _maxItems, float _minSize);
        void addItem(shared_ptr<N> _item);
        void split();
        void getChildren(shared_ptr<N> _item, vector<ofxOctreeNode*>& _vector);
        void queryBox(const glm::vec3& _bbMin, const glm::vec3& _bbMax, unordered_set<shared_ptr<N>>& _result);
        void queryRay(const glm::vec3& _origin, const glm::vec3& _direction, unordered_set<shared_ptr<N>>& _result);
        bool nodeIntersectBox(const glm::vec3& _bbMin, const glm::vec3& _bbMax);
        bool nodeIntersectRay(const glm::vec3& _origin, const glm::vec3& _direction);
        glm::vec3 bbMin, bbMax;
        vector<shared_ptr<N>> items;
        array<ofxOctreeNode*, 8>* children;
        int maxItems;
        float minSize;
};

template<class N>
ofxOctreeNode<N>::ofxOctreeNode() {
    children = nullptr;
}

template<class N>
ofxOctreeNode<N>::ofxOctreeNode(const glm::vec3& _bbMin, const glm::vec3& _bbMax, int _maxItems, float _minSize) {
    children = nullptr;
    bbMin = glm::vec3(MIN(_bbMin.x, _bbMax.x), MIN(_bbMin.y, _bbMax.y), MIN(_bbMin.z, _bbMax.z));
    bbMax = glm::vec3(MAX(_bbMin.x, _bbMax.x), MAX(_bbMin.y, _bbMax.y), MAX(_bbMin.z, _bbMax.z));
    maxItems = _maxItems;
    minSize = _minSize;
}

template<class N>
void ofxOctreeNode<N>::addItem(shared_ptr<N> _item) {
    if (children == nullptr) {
        items.push_back(_item);
        if (items.size() > maxItems) split();
    } else {
        vector<ofxOctreeNode*> intersectChildren;
        getChildren(_item, intersectChildren);
        for (ofxOctreeNode* child : intersectChildren) {
            if (child != nullptr) child->addItem(_item);
        }
    }
}

template<class N>
void ofxOctreeNode<N>::split() {
    if ((bbMax.x - bbMin.x) < minSize && (bbMax.y - bbMin.y) < minSize && (bbMax.z - bbMin.z) < minSize) return;
    float halfX = static_cast<float>(0.5 * (bbMax.x + bbMin.x));
    float halfY = static_cast<float>(0.5 * (bbMax.y + bbMin.y));
    float halfZ = static_cast<float>(0.5 * (bbMax.z + bbMin.z));
    children = new array<ofxOctreeNode*, 8>;
    (*children)[0] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMin.x, bbMin.y, bbMin.z), maxItems, minSize);
    (*children)[1] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMin.x, bbMin.y, bbMax.z), maxItems, minSize);
    (*children)[2] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMin.x, bbMax.y, bbMin.z), maxItems, minSize);
    (*children)[3] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMin.x, bbMax.y, bbMax.z), maxItems, minSize);
    (*children)[4] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMax.x, bbMin.y, bbMin.z), maxItems, minSize);
    (*children)[5] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMax.x, bbMin.y, bbMax.z), maxItems, minSize);
    (*children)[6] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMax.x, bbMax.y, bbMin.z), maxItems, minSize);
    (*children)[7] = new ofxOctreeNode(glm::vec3(halfX, halfY, halfZ), glm::vec3(bbMax.x, bbMax.y, bbMax.z), maxItems, minSize);
    for (shared_ptr<N> item : items) addItem(item);
    items.clear();
}

template<class N>
void ofxOctreeNode<N>::getChildren(shared_ptr<N> _item, vector<ofxOctreeNode*>& _vector) {
    if (_item->intersectBox(bbMin, bbMax)) {
        if (children != nullptr) {
            for (ofxOctreeNode<N>* child : *children) {
                if (_item->intersectBox(child->bbMin, child->bbMax)) child->getChildren(_item, _vector);
            }
        } else _vector.push_back(this);
    } else _vector.push_back(nullptr);
}

template<class N>
void ofxOctreeNode<N>::queryBox(const glm::vec3& _bbMin, const glm::vec3& _bbMax, unordered_set<shared_ptr<N>>& _result) {
    if (children == nullptr) {
        for (shared_ptr<N> node : items) _result.insert(node);
    } else {
        for (ofxOctreeNode* child : *children) {
            if (child->nodeIntersectBox(_bbMin, _bbMax)) child->queryBox(_bbMin, _bbMax, _result);
        }
    }
}

template<class N>
void ofxOctreeNode<N>::queryRay(const glm::vec3& _origin, const glm::vec3& _direction, unordered_set<shared_ptr<N>>& _result) {
    if (children == nullptr) {
        for (shared_ptr<N> node : items) _result.insert(node);
    } else {
        for (ofxOctreeNode* child : *children) {
            if (child->nodeIntersectRay(_origin, _direction)) child->queryRay(_origin, _direction, _result);
        }
    }
}

template<class N>
bool ofxOctreeNode<N>::nodeIntersectBox(const glm::vec3& _bbMin, const glm::vec3& _bbMax) {
    bool xCheck = (_bbMin.x < bbMax.x) && (_bbMax.x > bbMin.x);
    bool yCheck = (_bbMin.y < bbMax.y) && (_bbMax.y > bbMin.y);
    bool zCheck = (_bbMin.z < bbMax.z) && (_bbMax.z > bbMin.z);
    return xCheck && yCheck && zCheck;
}

template<class N>
bool ofxOctreeNode<N>::nodeIntersectRay(const glm::vec3& _origin, const glm::vec3& _direction) {
    glm::vec3 n = (bbMin - _origin) / _direction;
    glm::vec3 f = (bbMax - _origin) / _direction;
    glm::vec3 nMin = glm::vec3(MIN(n.x, f.x), MIN(n.y, f.y), MIN(n.z, f.z));
    glm::vec3 fMax = glm::vec3(MAX(n.x, f.x), MAX(n.y, f.y), MAX(n.z, f.z));
    float t0 = MAX(MAX(nMin.x, nMin.y), nMin.z);
    float t1 = MIN(MIN(fMax.x, fMax.y), fMax.z);
    if ((t0 < 1e-3 && t1 > 1e-3) || (t0 >= 1e-3 && t0 < t1)) return true;
    else return false;
}

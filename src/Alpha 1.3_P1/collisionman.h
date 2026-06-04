#pragma once
#include <vector>
#include "glm/glm.hpp"
#include "Scene.h"

struct BVHNode {
    glm::vec3 minbounds;
    glm::vec3 maxbounds;

    BVHNode* leftchild = nullptr;
    BVHNode* rightchild = nullptr;

    int nodeobjectid = -1;
};

extern bool detectcollision(glm::vec3& position, BVHNode* node);
void AABB(Object& obj);
void playercolcalc();
BVHNode* BuildBVH(std::vector<Object>& Objectlist);

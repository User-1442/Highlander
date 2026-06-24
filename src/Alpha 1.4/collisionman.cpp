#include <cmath>
#include "glm/glm.hpp"
#include "collisionman.h"
#include <iostream>
#include <algorithm>
#include "Scene.h"
#include "GUI.h"

float playerspeed = 3.0f;
float playerwidth = 1.0f;
float playerheight = 1.0f;
float playerdepth = 1.0f;

glm::vec3 origin = glm::vec3(0.0f);

glm::vec3 globalmincoords = glm::vec3(0.0f);
glm::vec3 globalmaxcoords = glm::vec3(0.0f);
glm::vec3 playerdefaultmaxcoords = glm::vec3(1, 1, 1);
glm::vec3 playerdefaultmincoords = glm::vec3(-1, -2, -1);
glm::vec3 playermaxcoords = glm::vec3(1, 1, 1);
glm::vec3 playermincoords = glm::vec3(-1, -2, -1);
glm::vec3 prevplayerposmax = glm::vec3(0.0f);
glm::vec3 prevplayerposmin = glm::vec3(0.0f);
glm::vec3 closestside = glm::vec3(0.0f);
glm::vec3 collisiondir = glm::vec3(0.0f);
glm::vec3 r = glm::vec3(1.0f, 2.0f, 1.0f);

std::vector<glm::vec3> playershape;
std::vector<glm::vec3> edges;
std::vector<glm::vec3> simplex;
std::vector<glm::vec3> discarded_simplex;

void playercolcalc() {
    playermaxcoords = glm::vec3(playerdefaultmaxcoords.x * playerwidth, playerdefaultmaxcoords.y * playerheight, playerdefaultmaxcoords.z * playerdepth);
    playermincoords = glm::vec3(playerdefaultmincoords.x * playerwidth, playerdefaultmincoords.y * playerheight, playerdefaultmincoords.z * playerdepth);

}

void AABB(Object& obj) {
    glm::vec3 prevmincoords = obj.vertices[0];
    glm::vec3 prevmaxcoords = obj.vertices[0];
    for (int i = 0; i < obj.vertices.size(); i++) {
        glm::vec3 currentcoords = glm::vec3(Models[obj.id] * glm::vec4(obj.vertices[i], 1.0f));
        prevmincoords.x = std::min(prevmincoords.x, currentcoords.x);
        prevmincoords.y = std::min(prevmincoords.y, currentcoords.y);
        prevmincoords.z = std::min(prevmincoords.z, currentcoords.z);
        prevmaxcoords.x = std::max(prevmaxcoords.x, currentcoords.x);
        prevmaxcoords.y = std::max(prevmaxcoords.y, currentcoords.y);
        prevmaxcoords.z = std::max(prevmaxcoords.z, currentcoords.z);
    }
    obj.colmincoords = prevmincoords;
    obj.colmaxcoords = prevmaxcoords;
    std::cout << "OBJ COLLISION MAX COORDS: " << obj.colmaxcoords.x << " " << obj.colmaxcoords.y << " " << obj.colmaxcoords.z << "\n" << std::endl;
    std::cout << "OBJ COLLISION MIN COORDS: " << obj.colmincoords.x << " " << obj.colmincoords.y << " " << obj.colmincoords.z << "\n" << std::endl;
}

int longestAxis(glm::vec3 mincoords, glm::vec3 maxcoords) {
    glm::vec3 subtractcoords = maxcoords - mincoords;
    float maxaxis = glm::max(glm::max(subtractcoords.x, subtractcoords.y), subtractcoords.z);
    if (maxaxis == subtractcoords.x) {
        return 0;
    }
    if (maxaxis == subtractcoords.y) {
        return 1;
    }
    if (maxaxis == subtractcoords.z) {
        return 2;
    } else {
        return -1;
    }
}

BVHNode* BuildBVH(std::vector<Object>& Objectlist) {
    std::cout << "Building BVH Tree \n";
    std::vector<Object> Leftlist;
    std::vector<Object> Rightlist;
    BVHNode* node = new BVHNode();
    node->minbounds = Objectlist[0].colmincoords + Objectlist[0].position;
    node->maxbounds = Objectlist[0].colmaxcoords + Objectlist[0].position;

    for (int i = 0; i < Objectlist.size(); i++) {
        glm::vec3 worldcolmin = Objectlist[i].colmincoords + Objectlist[i].position;
        glm::vec3 worldcolmax = Objectlist[i].colmaxcoords + Objectlist[i].position;
        if (worldcolmin.x < node->minbounds.x) {
            node->minbounds.x = worldcolmin.x;
        }
        if (worldcolmin.y < node->minbounds.y) {
            node->minbounds.y = worldcolmin.y;
        }
        if (worldcolmin.z < node->minbounds.z) {
            node->minbounds.z = worldcolmin.z;
        }
        if (worldcolmax.x > node->maxbounds.x) {
            node->maxbounds.x = worldcolmax.x;
        }
        if (worldcolmax.y > node->maxbounds.y) {
            node->maxbounds.y = worldcolmax.y;
        }
        if (worldcolmax.z > node->maxbounds.z) {
            node->maxbounds.z = worldcolmax.z;
        }
    }

    if (Objectlist.size() == 1) {
        node->nodeobjectid = Objectlist[0].id;
        return node;
    }

    int axis = longestAxis(node->minbounds, node->maxbounds);

    if (axis == 0) {
        for (int i = 0; i < Objectlist.size(); i++) {
            float midpoint = (node->minbounds.x + node->maxbounds.x) / 2.0;
            if (Objectlist[i].position.x < midpoint) {
                Leftlist.push_back(Objectlist[i]);
            } else {
                Rightlist.push_back(Objectlist[i]);
            }
        }
    }
    if (axis == 1) {
        for (int i = 0; i < Objectlist.size(); i++) {
            float midpoint = (node->minbounds.y + node->maxbounds.y) / 2.0;
            if (Objectlist[i].position.y < midpoint) {
                Leftlist.push_back(Objectlist[i]);
            } else {
                Rightlist.push_back(Objectlist[i]);
            }
        }
    }

    if (axis == 2) {
        for (int i = 0; i < Objectlist.size(); i++) {
            float midpoint = (node->minbounds.z + node->maxbounds.z) / 2.0;
            if (Objectlist[i].position.z < midpoint) {
                Leftlist.push_back(Objectlist[i]);
            } else {
                Rightlist.push_back(Objectlist[i]);
            }
        }
    }

    if (Leftlist.size() != 0 && Rightlist.size() != 0) {
        node->leftchild = BuildBVH(Leftlist);
        node->rightchild = BuildBVH(Rightlist);
        std::cout << "Children properly populated \n";
    } else {
       Leftlist.clear();
       Rightlist.clear();
       for (int i = 0; i < Objectlist.size(); i++) {
            if (i < Objectlist.size() / 2) {
                Leftlist.push_back(Objectlist[i]);
            } else {
                Rightlist.push_back(Objectlist[i]);
            }
       }
       node->rightchild = BuildBVH(Rightlist);
       node->leftchild = BuildBVH(Leftlist);
    }
    return node;
}

glm::vec3 findsupportpoint(std::vector<glm::vec3>& shape_p, glm::vec3 d) {
    float previousdotproduct = -FLT_MAX;
    int highestid = -1;
    for (int i = 0; i < shape_p.size(); i++) {
        glm::vec3 currentvertex = shape_p[i];
        float dotproduct = glm::dot(currentvertex, d);
        if (dotproduct > previousdotproduct) {
            previousdotproduct = dotproduct;
            highestid = i;
        }
    }
    return shape_p[highestid];
}

glm::vec3 find_elipse_support(glm::vec3 d, glm::vec3 position) {
    d = glm::normalize(d);
    glm::vec3 numerator = (d * (r * r));
    float denominator = glm::sqrt(((d.x * r.x) * (d.x * r.x)) + ((d.y * r.y) * (d.y * r.y)) + ((d.z * r.z) * (d.z * r.z)));
    glm::vec3 support = (numerator / denominator) + position;
    return support;
}

/*bool simplexsolver(std::vector<glm::vec3> simplex) {
    bool iscolliding;
    if (simplex.size() != 4) {
        iscolliding = false;
    }
}*/

bool Originchecker(std::vector<glm::vec3>& simplex, glm::vec3& Dir) {
    bool stilltrue = true;
    bool swapvertices = false;
    glm::vec3 A = simplex[0];
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[2];
    glm::vec3 D = simplex[3];
    glm::vec3 AB = simplex[1] - simplex[0];
    glm::vec3 AC = simplex[2] - simplex[0];
    glm::vec3 AD = simplex[3] - simplex[0];
    glm::vec3 BD = simplex[3] - simplex[1];
    glm::vec3 CD = simplex[3] - simplex[2];
    glm::vec3 side1 = glm::cross(AB, AC);
    glm::vec3 side2 = glm::cross(AB, AD);
    glm::vec3 side3 = glm::cross(AC, AD);
    glm::vec3 side4 = glm::cross(BD, CD);
    if (glm::dot(side1, D - A) > 0) {
        swapvertices = true;
        side1 = -side1; //normals are inverted. Fix it for EPA because it's a WIMP AND HAS NO BACKBONE
    }
    if (glm::dot(side2, C - A) > 0) {
        side2 = -side2;
    }
    if (glm::dot(side3, B - A) > 0) {
        side3 = -side3;
    }
    if (glm::dot(side4, A - B) > 0) {
        side4 = -side4;
    }
    if (glm::dot(side1, -A) < 0 && glm::dot(side2, -A) < 0 && glm::dot(side3, -A) < 0 && glm::dot(side4, -B) < 0) { //since the origin is 0,0,0 finding a vector that points to it can be simplified to just the negative of the vector.
        if (swapvertices == true) {
            //normals are inverted. Fix it for EPA because it's a WIMP AND HAS NO BACKBONE
            simplex.clear();
            simplex.push_back(A);
            simplex.push_back(C);
            simplex.push_back(B);
            simplex.push_back(D);
        } else {
            simplex.clear();
            simplex.push_back(A);
            simplex.push_back(B);
            simplex.push_back(C);
            simplex.push_back(D);
        } //also apparently swapping it once handles the whole shape! :D
        return true;
    } else {
        float maxdot = std::max({glm::dot(side1, -A), glm::dot(side2, -A), glm::dot(side3, -A), glm::dot(side4, -B)});
        if (maxdot == glm::dot(side1, -A)) {
            discarded_simplex.push_back(simplex[3]);
            simplex.erase(simplex.begin() + 3);
            Dir = side1;
        } else if (maxdot == glm::dot(side2, -A)) {
            discarded_simplex.push_back(simplex[2]);
            simplex.erase(simplex.begin() + 2);
            Dir = side2;
        } else if (maxdot == glm::dot(side3, -A)) {
            discarded_simplex.push_back(simplex[1]);
            simplex.erase(simplex.begin() + 1);
            Dir = side3;
        } else {
            discarded_simplex.push_back(simplex[0]);
            simplex.erase(simplex.begin());
            Dir = side4;
        }
        return false;
    }
}

std::vector<glm::vec3> facenormals(std::vector<unsigned int> faces) {
    if (faces.size() != 0) {
        std::vector<glm::vec3> simplexnormals;
        for (int i = 0; i < (faces.size() / 3); i++) {
            int startingpoint = i * 3;
            glm::vec3 AB = simplex[faces[startingpoint + 1]] - simplex[faces[startingpoint]];
            glm::vec3 AC = simplex[faces[startingpoint + 2]] - simplex[faces[startingpoint]];
            simplexnormals.push_back(glm::normalize(glm::cross(AB, AC)));
        }
        return simplexnormals;
    } else {
        std::vector<glm::vec3> nothing;
        nothing.push_back(glm::vec3(0.0f));
        return nothing;
    }
}

int findminface(std::vector<unsigned int> faces) {
    int minface = 0;
    float previousmindistance = FLT_MAX;
    for (int i = 0; i < (faces.size() / 3); i++) {
        int startingpoint = i * 3;
        glm::vec3 A = simplex[faces[startingpoint]];
        glm::vec3 B = simplex[faces[startingpoint + 1]];
        glm::vec3 C = simplex[faces[startingpoint + 2]];
        glm::vec3 activeface = glm::normalize(glm::cross(B - A, C - A));
        if (std::abs(glm::dot(activeface, A)) < previousmindistance) {
            previousmindistance = std::abs(glm::dot(activeface, A));
            minface = i;
        }
    }
    return minface;
}

bool indexrepeatchecker(std::vector<unsigned int> discarded_faces, glm::vec2 reverseindex) {
    for (int j = 0; j < (discarded_faces.size() / 3); j++) {
        if (discarded_faces[(j * 3)] == reverseindex.x) {
            if (discarded_faces[(j * 3) + 1] == reverseindex.y) {
                return false;
            }
        }
        if (discarded_faces[(j * 3) + 1] == reverseindex.x) {
            if (discarded_faces[(j * 3) + 2] == reverseindex.y) {
                return false;
            }
        }
        if (discarded_faces[(j * 3) + 2] == reverseindex.x) {
            if (discarded_faces[(j * 3)] == reverseindex.y) {
                return false;
            }
        }
    }
    return true;
}

glm::vec3 EPA(std::vector<glm::vec3> shape_p, std::vector<glm::vec3> shape_q, glm::vec3 position, bool elipse) {
    std::vector<unsigned int> faces = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };

    //auto auto formats it so the compiler just knows what variable it is?!?!?!
    auto polynormals = facenormals(faces);

    glm::vec3 minnormal = glm::vec3(FLT_MAX);
    float mindistance = FLT_MAX; //YOU CAN DO THAT?!?! Thx to the tutorial I was using on yt

    while (mindistance == FLT_MAX) {

        std::vector<unsigned int> newfaces;
        std::vector<unsigned int> discarded_faces;
        std::vector<unsigned int> horizonedges;

        int minface = findminface(faces);
        minnormal = polynormals[minface];

        glm::vec3 secondsupportpoint = elipse ? find_elipse_support(minnormal, position) : findsupportpoint(shape_q, -minnormal);
        glm::vec3 newpolytope = findsupportpoint(shape_p, minnormal) - secondsupportpoint;
        simplex.push_back(newpolytope);

        float newdist = glm::dot(newpolytope, minnormal); //haha variable sounds like something weird

        float potentialmindistance = glm::dot(minnormal, simplex[faces[minface * 3]]);

        float difference = newdist - potentialmindistance;

        if (difference > -0.0001f && difference < 0.0001f) {
            mindistance = potentialmindistance;
            return minnormal * mindistance;
        }
        for (int i = 0; i < simplex.size(); i++) {
            if (glm::distance(newpolytope, simplex[i]) < 0.0001f) {
                mindistance = potentialmindistance;
                return minnormal * mindistance;
            }
        }

        for (int i = 0; i < (faces.size() / 3); i++) {
            if (glm::dot(polynormals[i], newpolytope - simplex[faces[i * 3]]) < 0) {
                newfaces.push_back(faces[i * 3]);
                newfaces.push_back(faces[(i * 3) + 1]);
                newfaces.push_back(faces[(i * 3) + 2]);
            } else {
                discarded_faces.push_back(faces[i * 3]);
                discarded_faces.push_back(faces[(i * 3) + 1]);
                discarded_faces.push_back(faces[(i * 3) + 2]);
            }
        }
        for (int i = 0; i < (discarded_faces.size() / 3); i++) {
            glm::vec2 currentindice = glm::vec2(discarded_faces[(i * 3)], discarded_faces[(i * 3) + 1]);
            glm::vec2 currentindice2 = glm::vec2(discarded_faces[(i * 3) + 1], discarded_faces[(i * 3) + 2]);
            glm::vec2 currentindice3 = glm::vec2(discarded_faces[(i * 3) + 2], discarded_faces[(i * 3)]);
            glm::vec2 reverseindex = glm::vec2(currentindice.y, currentindice.x);
            glm::vec2 reverseindex2 = glm::vec2(currentindice2.y, currentindice2.x);
            glm::vec2 reverseindex3 = glm::vec2(currentindice3.y, currentindice3.x);
            bool isvalidindex = true;
            bool isvalidindex2 = true;
            bool isvalidindex3 = true;
            isvalidindex = indexrepeatchecker(discarded_faces, reverseindex);
            isvalidindex2 = indexrepeatchecker(discarded_faces, reverseindex2);
            isvalidindex3 = indexrepeatchecker(discarded_faces, reverseindex3);
            if (isvalidindex == true) {
                horizonedges.push_back(currentindice.x);
                horizonedges.push_back(currentindice.y);
            }
            if (isvalidindex2 == true) {
                horizonedges.push_back(currentindice2.x);
                horizonedges.push_back(currentindice2.y);
            }
            if (isvalidindex3 == true) {
                horizonedges.push_back(currentindice3.x);
                horizonedges.push_back(currentindice3.y);
            }
        }

        for (int i = 0; i < (horizonedges.size() / 2); i++) {
            newfaces.push_back(horizonedges[(i * 2)]);
            newfaces.push_back(simplex.size() - 1); //Order it like this so it runs counterclockwise. If you don't the normal will be messed up on the next pass.
            newfaces.push_back(horizonedges[(i * 2) + 1]);
        }

        if (faces.empty() == true || polynormals.empty() == true || newfaces.empty() == true) {
            launchie = false;
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        faces = newfaces;
        polynormals = facenormals(faces);
    }
    return glm::vec3(0.0f);
}

bool GJK(std::vector<glm::vec3> shape_p, std::vector<glm::vec3> shape_q, glm::vec3 position, bool elipse) {
    glm::vec3 previousvertex = glm::vec3(0.0f);
    simplex.clear();
    discarded_simplex.clear();
    bool searchedcollision = false;
    glm::vec3 D = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 secondsupportpoint = elipse ? find_elipse_support(-D, position) : findsupportpoint(shape_q, -D);
    glm::vec3 A = findsupportpoint(shape_p, D) - secondsupportpoint;
    simplex.push_back(A);
    D = -A;
    while (searchedcollision == false) {
        secondsupportpoint = elipse ? find_elipse_support(-D, position) : findsupportpoint(shape_q, -D);
        A = findsupportpoint(shape_p, D) - secondsupportpoint;
        for (int i = 0; i < simplex.size(); i++) {
            if (glm::distance(A, simplex[i]) < 0.001f) {
                return false;
            }
        }
        if (glm::dot(A, D) < 0.0001f) {
            searchedcollision = true;
            return false;
        }

        for (int i = 0; i < discarded_simplex.size(); i++) {
            if (A == discarded_simplex[i]) {
                return false;
            }
        }

        simplex.push_back(A);
        if (simplex.size() == 1) {
            D = origin - D;
        }
        if (simplex.size() == 2){
            previousvertex = simplex[simplex.size() - 2];
            glm::vec3 AB = simplex[simplex.size() - 1] - previousvertex;
            glm::vec3 AO = glm::vec3(0.0f) - A;
            glm::vec3 Across = glm::cross(AB, AO);
            D = glm::cross(AB, Across);
        }
        if (simplex.size() == 3) {
            glm::vec3 AB = A - simplex[1]; //but it also needs to be here because FU it won't ever build to 4 if it isn't here
            glm::vec3 AC = A - simplex[0];
            glm::vec3 AO = glm::vec3(0.0f) - A;
            D = glm::cross(AB, AC);
            if (glm::dot(D, AO) < 0) {
                D = -D;
            }
        }
        if (simplex.size() == 4) {
            if (Originchecker(simplex, D) == true) {
                searchedcollision = true;
                return true;
            }
        }
        if (simplex.size() > 4) {
            std::cout << "COLLISION ERROR MINKOWSKI SIMPLEX TOO BIG";
            return false;
        }
    }
    return false;
}

bool detectcollision(glm::vec3& position, BVHNode* node) {
    bool coldetected = false;
    std::vector<glm::vec3> worldspacevectors;
    glm::vec3 newposition = position;
    collisiondir = glm::vec3(0.0f);
    if (node == nullptr) {
        std::cout << "Null pointer error\n";
        launchie = false;
        return false;
    }
    glm::vec3 playerposmax = playermaxcoords + position;
    glm::vec3 playerposmin = playermincoords + position;
    if (playerposmin.x <= node->maxbounds.x && playerposmax.x >= node->minbounds.x && playerposmin.y <= node->maxbounds.y && playerposmax.y >= node->minbounds.y && playerposmin.z <= node->maxbounds.z && playerposmax.z >= node->minbounds.z) {
        if (node->nodeobjectid == -1) {
            if (detectcollision(newposition, node->leftchild) == true) {
                coldetected = true;
            }
            if (detectcollision(newposition, node->rightchild) == true) {
                coldetected = true;
            }
            if (coldetected == true) {
                position = newposition;
            }
            return coldetected;

        } else {
            std::vector<glm::vec3> worldspacevertices;
            for (int i = 0; i < Objects[node->nodeobjectid].vertices.size(); i++) {
                worldspacevertices.push_back(glm::vec3(Models[node->nodeobjectid] * glm::vec4(Objects[node->nodeobjectid].vertices[i], 1.0f)));
            }
            playershape.clear();
            if (GJK(worldspacevertices, playershape, position, true) == true) {
                collisiondir = EPA(worldspacevertices, playershape, position, true);
                position += collisiondir;
                simplex.clear();
                return true;
            } else {
                simplex.clear();
                return false;
            }
        }
    } else {
        return false;
    }
    return false;
}

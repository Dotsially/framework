#ifndef PHYSICS
#define PHYSICS

#include "common.cpp"

struct BoundingBoxAABB{
    glm::vec3 min; //top left
    glm::vec3 max; //bottom right
};

bool PhysicsCheckCollisionAABB(BoundingBoxAABB& first, BoundingBoxAABB& second){
    // Check for separation on x, y, and z axes
    bool collisionX = first.max.x >= second.min.x && second.max.x >= first.min.x;
    bool collisionY = first.max.y >= second.min.y && second.max.y >= first.min.y;
    bool collisionZ = first.max.z >= second.min.z && second.max.z >= first.min.z;
    
    return collisionX && collisionY && collisionZ;
}

// Returns the minimum translation vector
glm::vec3 PhysicsResolveCollisionAABB(BoundingBoxAABB& first, BoundingBoxAABB& second){
    // Calculate overlaps on each axis
    float overlapX1 = second.max.x - first.min.x;
    float overlapX2 = first.max.x - second.min.x;
    float overlapX = (overlapX1 < overlapX2) ? overlapX1 : -overlapX2;

    float overlapY1 = second.max.y - first.min.y;
    float overlapY2 = first.max.y - second.min.y;
    float overlapY = (overlapY1 < overlapY2) ? overlapY1 : -overlapY2;

    float overlapZ1 = second.max.z - first.min.z;
    float overlapZ2 = first.max.z - second.min.z;
    float overlapZ = (overlapZ1 < overlapZ2) ? overlapZ1 : -overlapZ2;

    // Find the axis with the smallest overlap (minimum translation)
    glm::vec3 mtv;

    float absX = std::abs(overlapX);
    float absY = std::abs(overlapY);
    float absZ = std::abs(overlapZ);

    if (absX < absY && absX < absZ) {
        mtv = glm::vec3(overlapX, 0.0f, 0.0f);
    } else if (absY < absZ) {
        mtv = glm::vec3(0.0f, overlapY, 0.0f);
    } else {
        mtv = glm::vec3(0.0f, 0.0f, overlapZ);
    }

    return mtv;
}

#endif
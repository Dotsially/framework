#ifndef RAYCAST
#define RAYCAST
#include "common.cpp"

#include "window.cpp"
#include "input.cpp"
#include "camera.cpp"

struct Ray{
    glm::vec3 origin;
    glm::vec3 direction;
};

Ray RaycastMouseWorldSpace(Camera* camera, Window* window, InputState* state) {
    // Convert the mouse position from window coordinates to Normalized Device Coordinates (NDC)
    // NDC ranges from -1 to 1 in both X and Y
    float ndcX = (2.0f * state->mousePosition.x) / window->m_width - 1.0f;
    float ndcY = 1.0f - (2.0f * state->mousePosition.y) / window->m_height; // Y is inverted because window coordinates start from top-left

    // Create a ray in clip space pointing into the screen (-1 on the Z axis)
    glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

    // Calculate the inverse of the projection and view matrices
    glm::mat4 invProjection = glm::inverse(camera->m_projectionMatrix);
    glm::mat4 invView = glm::inverse(camera->m_viewMatrix);

    // Transform the ray from clip space to eye (camera) space
    glm::vec4 rayEye = invProjection * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Set Z to -1 and W to 0 to represent a direction, not a point

    // Transform the ray from eye space to world space
    glm::vec4 rayWorld4 = invView * rayEye;

    // Normalize the direction vector (ignore the w component)
    glm::vec3 rayWorld = glm::normalize(glm::vec3(rayWorld4));

    // The origin of the ray is the camera's position
    glm::vec3 rayOrigin = camera->m_position;

    Ray ray = {};
    ray.direction = rayWorld;
    ray.origin = rayOrigin;

    return ray;
}

#endif 
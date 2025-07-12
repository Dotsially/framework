#ifndef CAMERA
#define CAMERA

#include "common.cpp"

struct Camera{
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 position;
    glm::vec3 target;
    glm::vec2 angle;
    float zoomLevel = 3.0f;
    float minZoom = 0.01f;
    float maxZoom = 6.0f;
    float fov = glm::radians(70.0f);
};

void CameraApplyMouseInput(Camera* camera, glm::vec2 mouseDelta) {
    camera->angle -= glm::vec2(glm::radians(mouseDelta.x), glm::radians(mouseDelta.y));

    // Clamp vertical rotation to 90/-90 degrees
    camera->angle.y = glm::clamp(camera->angle.y, glm::radians(-89.0f), glm::radians(89.0f));
}

void CameraUpdate(Camera* camera, glm::vec3& targetPosition, float& mouseScroll){
    camera->zoomLevel -= mouseScroll;
    camera->zoomLevel = glm::clamp(camera->zoomLevel, camera->minZoom, camera->maxZoom);
    
    camera->target = targetPosition;
    camera->position.x = glm::sin(camera->angle.x) * camera->zoomLevel * glm::cos(camera->angle.y) + camera->target.x;
    camera->position.y = ((camera->angle.y <= 0.0f)? 1 : -1) * glm::sin(camera->angle.y) * camera->zoomLevel * glm::sin(camera->angle.y) + camera->target.y;
    camera->position.z = glm::cos(camera->angle.x) * camera->zoomLevel * glm::cos(camera->angle.y) + camera->target.z;

    camera->viewMatrix = glm::lookAt(camera->position, camera->target, glm::vec3(0, 1, 0));
    camera->projectionMatrix = glm::perspective(camera->fov, 1280.0f/720.0f, 0.1f, 1000.0f);
}

glm::mat4 CameraViewProjectionMatrix(Camera* camera){
    return camera->projectionMatrix * camera->viewMatrix;
}

#endif
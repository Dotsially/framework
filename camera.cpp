#ifndef CAMERA
#define CAMERA

#include "common.cpp"
#include "window.cpp"

struct Camera{
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec2 m_angle;
    float m_zoomLevel = 3.0f;
    float m_minZoom = 0.01f;
    float m_maxZoom = 10.0f;
    float m_fov = glm::radians(70.0f);

    void ApplyMouseInput(glm::vec2 mouseDelta) {
        m_angle -= glm::vec2(glm::radians(mouseDelta.x), glm::radians(mouseDelta.y));

        // Clamp vertical rotation to 90/-90 degrees
        m_angle.y = glm::clamp(m_angle.y, glm::radians(-89.0f), glm::radians(89.0f));
    }

    void Update(glm::vec3& targetPosition, float& mouseScroll){
        Window& window = window.Instance();

        m_zoomLevel -= mouseScroll;
        m_zoomLevel = glm::clamp(m_zoomLevel, m_minZoom, m_maxZoom);
        
        m_target = targetPosition;
        m_position.x = glm::sin(m_angle.x) * m_zoomLevel * glm::cos(m_angle.y) + m_target.x;
        m_position.y = ((m_angle.y <= 0.0f)? 1 : -1) * glm::sin(m_angle.y) * m_zoomLevel * glm::sin(m_angle.y) + m_target.y;
        m_position.z = glm::cos(m_angle.x) * m_zoomLevel * glm::cos(m_angle.y) + m_target.z;

        m_viewMatrix = glm::lookAt(m_position, m_target, glm::vec3(0, 1, 0));
        m_projectionMatrix = glm::perspective(m_fov, window.m_width/float(window.m_height), 0.1f, 1000.0f);
    }

    glm::mat4 ViewProjectionMatrix(){
        return m_projectionMatrix * m_viewMatrix;
    }
};


#endif
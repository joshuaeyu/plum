#include <plum/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 pos, float pitch, float yaw) 
    : Position(pos), Pitch(pitch), Yaw(yaw) {
        updateCameraVectors();  // recalculates Front(Pitch,Yaw), Up, and Right
    }

glm::mat4 Camera::GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, worldUp);
    }

void Camera::ProcessMouse(double xoffset, double yoffset) {
    Yaw     += xoffset * sensitivity;   // + = clockwise
    Pitch   += yoffset * sensitivity;   // + = upward
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;
    updateCameraVectors();
}

void Camera::ProcessKeyboard(Direction dir, float dt) {
    float dist = speed * dt;
    switch (dir) {
        case Direction::FORWARD:
            Position += Front * dist;
            break;
        case Direction::BACKWARD:
            Position -= Front * dist;
            break;
        case Direction::LEFT:
            Position -= Right * dist;
            break;
        case Direction::RIGHT:
            Position += Right * dist;
            break;
        case Direction::UP:
            Position += worldUp * dist;
            break;
        case Direction::DOWN:
            Position -= worldUp * dist;
            break;
    }
}

void Camera::updateCameraVectors() {
    // update Front - using Yaw and Pitch
    Front.x = cos( glm::radians(Yaw) ) * cos( glm::radians(Pitch) );
    Front.y = sin( glm::radians(Pitch) );
    Front.z = sin( glm::radians(Yaw) ) * cos( glm::radians(Pitch) );
    Front = glm::normalize(Front);
    // update Right and up - using new Front
    Right = glm::normalize( glm::cross(Front, worldUp) );
    Up    = glm::normalize( glm::cross(Right, Front) );
}

void Camera::updateCameraVectors(glm::vec3 newFront) {
    // update Right and up - using new Front
    Right = glm::normalize( glm::cross(Front, worldUp) );
    Up    = glm::normalize( glm::cross(Right, Front) );
}
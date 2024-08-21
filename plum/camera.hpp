#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum direction_t {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// camera settings
const float speed       = 5.f;
const float sensitivity = 0.1f;
const float zoom        = 45.0f;
const glm::vec3 worldUp = glm::vec3(0,1,0);

class Camera {
    public:

        Camera(glm::vec3 in_pos = glm::vec3(0,0,3), float in_pitch = 0.0f, float in_yaw = -90.0f) {
            position    = in_pos;
            pitch       = in_pitch; // relative to x-z plane, upward! (x-z toward +y)
            yaw         = in_yaw;   // relative to +x axis, clockwise! (+x toward +z)
            updateCameraVectors();  // recalculates front(pitch,yaw), up, and right
        }

        void UpdatePosX(float X) {
            position.x = X;
        }
        void UpdatePosY(float Y) {
            position.y = Y;
        }
        void UpdatePosZ(float Z) {
            position.z = Z;
        }
        void SetFront(glm::vec3 newfront) {
            front = newfront;
        }
        void SetProjectionMatrix(glm::mat4 proj) {
            projection = proj;
        }

        glm::vec3 GetPosition() { return position; }
        glm::vec3 GetFront() { return front; }
        glm::vec3 GetRight() { return right; }
        glm::vec3 GetUp() { return up; }
        glm::mat4 GetViewMatrix() {
            // position + front = target
            return glm::lookAt(position, position + front, worldUp);
        }
        glm::mat4 GetProjectionMatrix() { return projection; }

        void ProcessMouse(double xoffset, double yoffset) {
            yaw     += xoffset * sensitivity;   // + = clockwise
            pitch   += yoffset * sensitivity;   // + = upward
            if (pitch > 89.0f) {
                pitch = 89.0f;
            }
            if (pitch < -89.0f) {
                pitch = -89.0f;
            }
            updateCameraVectors();
        }

        void ProcessKeyboard(enum direction_t dir, float dt) {
            float dist = speed * dt;
            switch (dir) {
                case FORWARD:
                    position += front * dist;
                    break;
                case BACKWARD:
                    position -= front * dist;
                    break;
                case LEFT:
                    position -= right * dist;
                    break;
                case RIGHT:
                    position += right * dist;
                    break;
                case UP:
                    position += worldUp * dist;
                    break;
                case DOWN:
                    position -= worldUp * dist;
                    break;
            }
        }

    private:

        // transforms
        // glm::mat4 view, projection;
        glm::mat4 projection;
        // camera vectors
        glm::vec3 position, front, up, right;
        // angles
        float pitch, yaw;

        void updateCameraVectors() {
            // update front - using yaw and pitch
            front.x = cos( glm::radians(yaw) ) * cos( glm::radians(pitch) );
            front.y = sin( glm::radians(pitch) );
            front.z = sin( glm::radians(yaw) ) * cos( glm::radians(pitch) );
            front = glm::normalize(front);
            // update right and up - using new front
            right = glm::normalize( glm::cross(front, worldUp) );
            up    = glm::normalize( glm::cross(right, front) );
        }

        void updateCameraVectors(glm::vec3 newfront) {
            // update right and up - using new front
            right = glm::normalize( glm::cross(front, worldUp) );
            up    = glm::normalize( glm::cross(right, front) );
        }
};

#endif
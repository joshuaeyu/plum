#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    public:
        enum class Direction {
            FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
        };

        glm::mat4 Projection;
        glm::vec3 Position, Front, Up, Right;
        float Pitch; // relative to x-z plane, upward! (x-z toward +y)
        float Yaw;  // relative to +x axis, clockwise! (+x toward +z)

        Camera(glm::vec3 pos = glm::vec3(0,0,3), float pitch = 0.0f, float yaw = -90.0f) 
        : Position(pos), Pitch(pitch), Yaw(yaw) {
            updateCameraVectors();  // recalculates Front(Pitch,Yaw), Up, and Right
        }

        glm::mat4 GetViewMatrix() {
            return glm::lookAt(Position, Position + Front, worldUp);
        }

        void ProcessMouse(double xoffset, double yoffset) {
            Yaw     += xoffset * sensitivity;   // + = clockwise
            Pitch   += yoffset * sensitivity;   // + = upward
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
            updateCameraVectors();
        }

        void ProcessKeyboard(Direction dir, float dt) {
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

    private:
        float speed       = 5.f;
        float sensitivity = 0.1f;
        // float zoom        = 45.0f;
        glm::vec3 worldUp = glm::vec3(0,1,0);

        void updateCameraVectors() {
            // update Front - using Yaw and Pitch
            Front.x = cos( glm::radians(Yaw) ) * cos( glm::radians(Pitch) );
            Front.y = sin( glm::radians(Pitch) );
            Front.z = sin( glm::radians(Yaw) ) * cos( glm::radians(Pitch) );
            Front = glm::normalize(Front);
            // update Right and up - using new Front
            Right = glm::normalize( glm::cross(Front, worldUp) );
            Up    = glm::normalize( glm::cross(Right, Front) );
        }

        void updateCameraVectors(glm::vec3 newFront) {
            // update Right and up - using new Front
            Right = glm::normalize( glm::cross(Front, worldUp) );
            Up    = glm::normalize( glm::cross(Right, Front) );
        }
};

#endif
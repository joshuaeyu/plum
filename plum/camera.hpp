#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
    public:
        enum class Direction {
            FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
        };

        glm::mat4 Projection;
        glm::vec3 Position, Front, Up, Right;
        float Pitch; // relative to x-z plane, upward! (x-z toward +y)
        float Yaw;  // relative to +x axis, clockwise! (+x toward +z)

        Camera(glm::vec3 pos = glm::vec3(0,0,3), float pitch = 0.0f, float yaw = -90.0f);
        glm::mat4 GetViewMatrix();
        void ProcessMouse(double xoffset, double yoffset);
        void ProcessKeyboard(Direction dir, float dt);

    private:
        float speed       = 5.f;
        float sensitivity = 0.1f;
        // float zoom        = 45.0f;
        glm::vec3 worldUp = glm::vec3(0,1,0);

        void updateCameraVectors();
        void updateCameraVectors(glm::vec3 newFront);
};

#endif
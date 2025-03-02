#include <plum/component/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace Component {

    Camera::Camera(Transform transform, glm::mat4 projection) 
        : transform(transform), projection(projection),
        inputManager({GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT}) {}

    void Camera::ProcessInputs() {
        ProcessMouse(inputManager.GetMouseDeltaX(), inputManager.GetMouseDeltaY());

        if (inputManager.GetKeyDown(GLFW_KEY_W))
            ProcessKeyboard(Direction::Forward);
        if (inputManager.GetKeyDown(GLFW_KEY_S))
            ProcessKeyboard(Direction::Backward);
        if (inputManager.GetKeyDown(GLFW_KEY_A))
            ProcessKeyboard(Direction::Left);
        if (inputManager.GetKeyDown(GLFW_KEY_D))
            ProcessKeyboard(Direction::Right);
        if (inputManager.GetKeyDown(GLFW_KEY_SPACE))
            ProcessKeyboard(Direction::Up);
        if (inputManager.GetKeyDown(GLFW_KEY_LEFT_SHIFT))
            ProcessKeyboard(Direction::Down);
    }

    void Camera::ProcessMouse(double xoffset, double yoffset) {
        float deltaYaw = xoffset * sensitivity;
        float deltaPitch = yoffset * sensitivity;
        transform.Rotate(deltaYaw, deltaPitch, 0);

        glm::vec3 eulerAngles = transform.rotationEuler;
        if (eulerAngles.x > 89.0f) {
            transform.rotationEuler.x = 89.0f;
            transform.Update();
        }
        if (eulerAngles.x < -89.0f) {
            transform.rotationEuler.x = -89.0f;
            transform.Update();
        }
    }

    void Camera::ProcessKeyboard(Direction dir) {
        float dist = speed * Context::Window::DeltaTime();
        switch (dir) {
            case Direction::Forward:
                transform.Translate(transform.Front() * dist);
                break;
            case Direction::Backward:
                transform.Translate(-transform.Front() * dist);
                break;
            case Direction::Left:
                transform.Translate(-transform.Right() * dist);
                break;
            case Direction::Right:
                transform.Translate(transform.Right() * dist);
                break;
            case Direction::Up:
                transform.Translate(worldUp * dist);
                break;
            case Direction::Down:
                transform.Translate(-worldUp * dist);
                break;
        }
    }

}
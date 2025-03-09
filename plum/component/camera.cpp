#include <plum/component/camera.hpp>
#include <plum/context/window.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Component {

    Camera::Camera() 
        : Camera(Transform(), glm::perspective(45.f, 1.f, 0.1f, 100.f)) 
    {}

    Camera::Camera(Transform transform, glm::mat4 projection) 
        : transform(transform), 
        projection(projection),
        view(View()),
        inputObserver(Context::WindowInputsAndEventsManager::CreateInputObserver({GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT}))
    {}

    void Camera::ProcessInputs() {
        // Mouse: Rotation
        float deltaYaw = -inputObserver->GetCursorDeltaX() * sensitivity;
        float deltaPitch = inputObserver->GetCursorDeltaY() * sensitivity;
        rotate(deltaYaw, deltaPitch);
        
        // WASD/Space/Shift: Translation
        float distance = speed * Context::Window::DeltaTime();
        if (inputObserver->GetKeyDown(GLFW_KEY_W))
            translate(Direction::Forward, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_S))
            translate(Direction::Backward, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_A))
            translate(Direction::Left, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_D))
            translate(Direction::Right, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_SPACE))
            translate(Direction::Up, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_LEFT_SHIFT))
            translate(Direction::Down, distance);
    }

    const glm::mat4& Camera::View() {
        view = glm::lookAt(transform.position, transform.position + transform.Front(), worldUp);
        return view;
    }

    void Camera::rotate(double delta_yaw, double delta_pitch) {
        transform.Rotate(delta_pitch, delta_yaw, 0);

        glm::vec3 eulerAngles = transform.rotationEuler;
        if (eulerAngles.x > 0.99f * glm::half_pi<float>()) {
            transform.rotationEuler.x = 0.99f * glm::half_pi<float>();
            transform.Update();
        }
        if (eulerAngles.x < -0.99f * glm::half_pi<float>()) {
            transform.rotationEuler.x = -0.99f * glm::half_pi<float>();
            transform.Update();
        }
    }

    void Camera::translate(Direction dir, float dist) {
        switch (dir) {
            case Direction::Forward:
                transform.Translate(transform.Front() * dist);
                break;
            case Direction::Backward:
                transform.Translate(-transform.Front() * dist);
                break;
            case Direction::Left:
                transform.Translate(transform.Right() * dist);  // Because using <0,0,1> as front with RH coordinate system
                break;
            case Direction::Right:
                transform.Translate(-transform.Right() * dist); // Because using <0,0,1> as front with RH coordinate system
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
#include <plum/component/camera.hpp>
#include <plum/context/application.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Component {

    Camera::Camera() 
        : Camera(Transform(), glm::perspective(45.f, Context::Application::Instance().activeWindow->Aspect(), 0.1f, 100.f)) 
    {}

    Camera::Camera(const Transform& transform, const glm::mat4& projection) 
        : transform(transform), 
        projection(projection),
        view(View()),
        inputObserver(Context::WindowInputsAndEventsManager::CreateInputObserver({GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT})),
        eventListener(Context::WindowInputsAndEventsManager::CreateEventListener())
    {
        std::function<void(int,int)> staticFunc = std::bind(&Camera::framebufferSizeCallback, this, std::placeholders::_1, std::placeholders::_2);   
        eventListener->SetFramebufferSizeCallback(staticFunc);
    }

    void Camera::ProcessInputs() {
        // Mouse: Rotation
        float deltaYaw = -inputObserver->GetCursorDeltaX() * sensitivity;
        float deltaPitch = -inputObserver->GetCursorDeltaY() * sensitivity;
        Rotate(deltaYaw, deltaPitch);
        
        // WASD/Space/Shift: Translation
        float distance = speed * Context::Application::Instance().DeltaTime();
        if (inputObserver->GetKeyDown(GLFW_KEY_W))
            Translate(Direction::Forward, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_S))
            Translate(Direction::Backward, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_A))
            Translate(Direction::Left, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_D))
            Translate(Direction::Right, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_SPACE))
            Translate(Direction::Up, distance);
        if (inputObserver->GetKeyDown(GLFW_KEY_LEFT_SHIFT))
            Translate(Direction::Down, distance);
    }

    const glm::mat4& Camera::View() {
        view = glm::lookAt(transform.position, transform.position - transform.Front(), transform.Up());
        return view;
    }

    void Camera::SetRotation(float yaw, float pitch) {
        this->yaw = yaw;
        this->pitch = glm::clamp(pitch, -89.5f, 89.5f);
        
        transform.rotationQuat = glm::angleAxis(glm::radians(this->yaw), glm::vec3(0,1,0)) * glm::angleAxis(glm::radians(this->pitch), glm::vec3(1,0,0));
        transform.Update();
    }

    void Camera::Rotate(float delta_yaw, float delta_pitch) {
        SetRotation(yaw + delta_yaw, pitch + delta_pitch);  
    }

    void Camera::Translate(Direction dir, float dist) {
        switch (dir) {
            case Direction::Forward:
                transform.Translate(-transform.Front() * dist); // Camera faces in -z direction, but transform.Front() is +z
                break;
            case Direction::Backward:
                transform.Translate(transform.Front() * dist);  // Camera faces in -z direction, but transform.Front() is +z
                break;
            case Direction::Left:
                transform.Translate(-transform.Right() * dist);
                break;
            case Direction::Right:
                transform.Translate(transform.Right() * dist);
                break;
            case Direction::Up:
                transform.Translate(transform.Up() * dist);
                break;
            case Direction::Down:
                transform.Translate(-transform.Up() * dist);
                break;
        }
    }

    void Camera::framebufferSizeCallback(int width, int height) {
        projection = glm::perspective(45.f, static_cast<float>(width)/height, 0.1f, 100.f);
    }

}
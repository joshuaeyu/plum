#include <plum/component/camera.hpp>
#include <plum/context/application.hpp>
#include <plum/util/time.hpp>

#include <glm/glm.hpp>
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
        inputObserver(Context::InputsAndEventsManager::CreateInputObserver({GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_SPACE, GLFW_KEY_LEFT_SHIFT})),
        eventListener(Context::InputsAndEventsManager::CreateEventListener())
    {
        std::function<void(int,int)> staticFunc = std::bind(&Camera::framebufferSizeCallback, this, std::placeholders::_1, std::placeholders::_2);   
        eventListener->SetFramebufferSizeCallback(staticFunc);
    }

    void Camera::ProcessInputs() {
        if (!inputObserver->GetCursorEnabled())
            return;
        
        // Mouse: Rotation
        float deltaPitch = -inputObserver->GetCursorDeltaY() * sensitivity;
        float deltaYaw = -inputObserver->GetCursorDeltaX() * sensitivity;
        Rotate(deltaPitch, deltaYaw);
        
        // WASD/Space/Shift: Translation
        float distance = speed * Time::DeltaTime();
        if (inputObserver->GetKeyDown(GLFW_KEY_W))
            Translate(distance * -transform.Front()); // camera faces -z
        if (inputObserver->GetKeyDown(GLFW_KEY_S))
            Translate(distance * transform.Front()); // camera faces -z
        if (inputObserver->GetKeyDown(GLFW_KEY_A))
            Translate(distance * -transform.Right());
        if (inputObserver->GetKeyDown(GLFW_KEY_D))
            Translate(distance * transform.Right());
        if (inputObserver->GetKeyDown(GLFW_KEY_SPACE))
            Translate(distance * transform.Up());
        if (inputObserver->GetKeyDown(GLFW_KEY_LEFT_SHIFT))
            Translate(distance * -transform.Up());
    }

    const glm::mat4& Camera::View() {
        view = glm::lookAt(transform.position, transform.position - transform.Front(), transform.Up());
        return view;
    }

    void Camera::SetRotation(float pitch, float yaw) {
        this->pitch = glm::clamp(pitch, -89.5f, 89.5f);
        this->yaw = yaw;
        
        transform.rotationEuler = glm::vec3(this->pitch, this->yaw, 0);
        transform.Update();
    }

    void Camera::Rotate(float delta_pitch, float delta_yaw) {
        SetRotation(pitch + delta_pitch, yaw + delta_yaw);
    }

    void Camera::Translate(const glm::vec3& displacement) {
        transform.Translate(displacement);
    }

    void Camera::Translate(float dx, float dy, float dz) {
        transform.Translate({dx,dy,dz});
    }

    void Camera::framebufferSizeCallback(int width, int height) {
        projection = glm::perspective(45.f, static_cast<float>(width)/height, 0.1f, 100.f);
    }

}
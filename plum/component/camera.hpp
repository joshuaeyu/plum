#pragma once

#include <plum/context/inputsevents.hpp>
#include <plum/util/transform.hpp>

#include <glm/glm.hpp>

namespace Component {

    // May want to move to Scene namespace
    class Camera {
        public:
            Camera();
            Camera(const Transform& transform, const glm::mat4& projection);

            void ProcessInputs();

            void SetRotation(float yaw, float pitch);
            void Rotate(float delta_yaw, float delta_pitch);
            void Translate(const glm::vec3& displacement);
            void Translate(float dx, float dy, float dz);
            
            const glm::mat4& View();
            
            Transform transform;
            
            glm::mat4 projection;
            
            float speed       = 5.f;
            float sensitivity = 0.5f;
            
            // float zoom        = 45.0f;
            
        private:
            void framebufferSizeCallback(int width, int height);

            float pitch, yaw;
            glm::mat4 view;

            std::shared_ptr<Context::InputObserver> inputObserver;
            std::shared_ptr<Context::EventListener> eventListener;
    };

}
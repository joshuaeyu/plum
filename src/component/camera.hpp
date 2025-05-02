#pragma once

#include "context/inputsevents.hpp"
#include "util/transform.hpp"

#include <glm/glm.hpp>

namespace Component {

    // May want to move to Scene namespace
    class Camera {
        public:
            Camera();
            Camera(const Transform& transform, const glm::mat4& projection);

            Transform transform;
            glm::mat4 projection;
            float speed       = 5.f;
            float sensitivity = 0.5f;

            const glm::mat4& View();

            void SetRotation(float yaw, float pitch);
            void Rotate(float delta_yaw, float delta_pitch);
            void Translate(const glm::vec3& displacement);
            void Translate(float dx, float dy, float dz);
            void ProcessInputs();
            
        private:
            void framebufferSizeCallback(int width, int height);

            glm::mat4 view;
            float pitch, yaw;
            std::shared_ptr<Context::InputObserver> inputObserver;
            std::shared_ptr<Context::EventListener> eventListener;
    };

}
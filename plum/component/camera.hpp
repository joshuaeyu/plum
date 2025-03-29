#pragma once

#include <plum/context/inputsevents.hpp>
#include <plum/util/transform.hpp>

#include <glm/glm.hpp>

namespace Component {

    // May want to move to Scene namespace
    class Camera {
        public:
            enum class Direction {
                Forward, Backward, Left, Right, Up, Down
            };

            Camera();
            Camera(const Transform& transform, const glm::mat4& projection);

            void ProcessInputs();

            void SetRotation(float yaw, float pitch);
            void Rotate(float delta_yaw, float delta_pitch);
            void Translate(Direction dir, float dist);
            
            const glm::mat4& View();
            
            Transform transform;
            
            glm::mat4 projection;
            
            float speed       = 5.f;
            float sensitivity = 0.5f;
            
            // float zoom        = 45.0f;
            
        private:
            void framebufferSizeCallback(int width, int height);

            float yaw, pitch;
            glm::mat4 view;

            std::shared_ptr<Context::WindowInputObserver> inputObserver;
            std::shared_ptr<Context::WindowEventListener> eventListener;
    };

}
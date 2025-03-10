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
            Camera(Transform transform, glm::mat4 projection);

            void ProcessInputs();
            
            Transform transform;
            float speed       = 5.f;
            float sensitivity = 0.5f;
            
            const glm::mat4& View();
            glm::mat4 projection;
            // float zoom        = 45.0f;
            
        private:
            void rotate(double delta_yaw, double delta_pitch);
            void translate(const Direction dir, float dist);

            glm::mat4 view;
            glm::vec3 worldUp = glm::vec3(0,1,0);

            std::shared_ptr<Context::WindowInputObserver> inputObserver;
    };

}
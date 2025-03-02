#pragma once

#include <glm/glm.hpp>
#include <plum/context/inputmanager.hpp>
#include <plum/util/transform.hpp>

namespace Component {

    class Camera {
        public:
            enum class Direction {
                Forward, Backward, Left, Right, Up, Down
            };

            Transform transform;
            float speed       = 5.f;
            float sensitivity = 0.1f;
            
            glm::mat4 projection;
            // float zoom        = 45.0f;

            Camera(Transform transform, glm::mat4 projection);
            
            void ProcessInputs();
            void ProcessMouse(double xoffset, double yoffset);
            void ProcessKeyboard(Direction dir);

        private:
            glm::vec3 worldUp = glm::vec3(0,1,0);

            Context::InputManager inputManager;
    };

}
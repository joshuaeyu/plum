#pragma once

#include <plum/component/component.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


namespace Component {

    class Light : public ComponentBase {
        public:
            glm::vec3 color = glm::vec3(1);
            float intensity = 1.0f;
            
            virtual void Draw(const glm::mat4& parent_transform) override = 0;

            // Accessors
            float FarPlane() const;
            bool HasShadows() const;  
            // Modifiers
            void DisableShadows();

        protected:
            bool hasShadows = false;
            float nearPlane = 0.1;
            float farPlane = 50;

            Light(ComponentType type);
            virtual ~Light() = default;
    };

    class DirectionalLight : public Light {
        public:            
            DirectionalLight();

            void Draw(const glm::mat4& parent_transform) override;
            
            void EnableShadows(float width = 50.0f, float height = 50.0f, float near = 0.1f, float far = 50.0f, float dist = 20.0f);

            glm::mat4& LightspaceMatrix();

        private:
            glm::mat4 lightspaceMatrix = glm::identity<glm::mat4>();
            float projWidth = 50.0;
            float projHeight = 50.0;
            float distance = 20.0;

            void updateLightspaceMatrix(glm::vec3 direction);
            
    };

    class PointLight : public Light {
        public:
            PointLight();

            void Draw(const glm::mat4& parent_transform) override;

            void EnableShadows(float aspect = 1.0f, float near = 0.1f, float far = 75.0f);

            void SetAttenuation(float constant, float linear, float quadratic);

            std::vector<glm::mat4>& LightspaceMatrices();
            float Radius() const;
            float AttenuationConstant() const;
            float AttenuationLinear() const;
            float AttenuationQuadratic() const;

        private:
            float radius = 5;
            float attenuationConstant = 1.0;       // Default range 50
            float attenuationLinear = 0.09;        // Default range 50
            float attenuationQuadratic = 0.032;    // Default range 50
            std::vector<glm::mat4> lightspaceMatrices;
            float aspectRatio = 1.0;
            
            void updateLightspaceMatrices(glm::vec3 position);

            float updateRadius();
    };

}
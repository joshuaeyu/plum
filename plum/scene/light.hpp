#pragma once

#include <plum/scene/scenenode.hpp>

#include <vector>

namespace Component {

    inline static const glm::vec3 UP = glm::vec3(0,1,0);
    inline static const glm::vec3 DOWN = glm::vec3(0,-1,0);
    inline static const glm::vec3 LEFT = glm::vec3(-1,0,0);
    inline static const glm::vec3 RIGHT = glm::vec3(1,0,0);
    inline static const glm::vec3 FRONT = glm::vec3(0,0,1);
    inline static const glm::vec3 BACK = glm::vec3(0,0,-1);

    class Light : public SceneObject {
        public:
            glm::vec3 color = glm::vec3(1);
            
            virtual void Draw();
            virtual void Draw(const glm::mat4& parent_transf);

            virtual ~Light() {}

            // Accessors
            float GetFarPlane() const;
            bool HasShadows() const;  
            // Modifiers
            void DisableShadows();

        protected:
            bool hasShadows = false;
            float nearPlane = 0.1;
            float farPlane = 50;

            Light(const SceneObjectType type);

    };

    class DirectionalLight : public Light {
        public:
            glm::vec3 direction = glm::vec3(-1);
            
            DirectionalLight();

            void Draw() override;
            void Draw(const glm::mat4& parent_transf) override;
            
            // Modifiers
            void EnableShadows(const float width = 50.0f, const float height = 50.0f, const float near = 0.1f, const float far = 50.0f, const float dist = 20.0f);
            // Accessors
            glm::mat4& GetLightspaceMatrix();

        private:
            glm::mat4 lightspaceMatrix = glm::identity<glm::mat4>();
            float projWidth = 50.0;
            float projHeight = 50.0;
            float distance = 20.0;

            void updateLightspaceMatrix();
            
    };

    class PointLight : public Light {
        public:
            glm::vec3 position = glm::vec3(0);

            PointLight();

            void Draw() override;
            void Draw(const glm::mat4& parent_transf) override;

            // Methods     
            void EnableShadows(const float aspect = 1.0f, const float near = 0.1f, const float far = 75.0f);
            // Modifiers
            void SetAttenuation(float constant, float linear, float quadratic);
            // Accessors
            std::vector<glm::mat4>& GetLightspaceMatrices();
            float GetRadius() const;
            float GetAttenuationConstant() const;
            float GetAttenuationLinear() const;
            float GetAttenuationQuadratic() const;

        private:
            float radius = 5;
            float attenuationConstant = 1.0;       // Default range 50
            float attenuationLinear = 0.09;        // Default range 50
            float attenuationQuadratic = 0.032;    // Default range 50
            std::vector<glm::mat4> lightspaceMatrices;
            float aspectRatio = 1.0;
            
            void updateLightspaceMatrices();

            float updateRadius();
    };

}
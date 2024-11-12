#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <iostream>
#include <vector>
#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <plum/scenenode.hpp>
#include <plum/shape.hpp>

class Shader;

class Light {
    public:
        std::string Name, NameTemp;
        glm::vec3 Color = glm::vec3(1);
        static Shader* DefaultShader;
        
        virtual ~Light() {}

        // Accessors
        float GetFarPlane() const;
        bool HasShadows() const;  
        // Modifiers
        void SetName(std::string name);
        void DisableShadows();

    protected:
        bool hasShadows = false;
        float nearPlane = 0.1;
        float farPlane = 50;

        static unsigned int inline count = 0;

        Light(std::string type);

};

class DirectionalLight : public Light {
    public:
        glm::vec3 Direction = glm::vec3(-1);
        
        DirectionalLight();
        
        // Modifiers
        void EnableShadows(const float width = 50.0f, const float height = 50.0f, const float near = 0.1f, const float far = 50.0f, const float dist = 20.0f);
        // Accessors
        glm::mat4& GetLightspaceMatrix();

    private:
        glm::mat4 lightspaceMatrix = glm::identity<glm::mat4>();
        float projWidth = 50.0;
        float projHeight = 50.0;
        float distance = 20.0;
        glm::vec3 directionLast = glm::vec3(-1);

        void updateLightspaceMatrix();
        
};

class PointLight : public Light {
    public:
        glm::vec3 Position = glm::vec3(0);
        glm::vec3 Size = glm::vec3(0.1);

        PointLight();

        // Methods     
        void DrawDefault();
        void Draw(Shader& shader);
        void EnableShadows(const float aspect = 1.0f, const float near = 0.1f, const float far = 75.0f);
        // Modifiers
        void SetAttenuation(float constant, float linear, float quadratic);
        // Accessors
        std::shared_ptr<Sphere> GetShapeTemplate();
        std::shared_ptr<SceneNode> GetSceneNode();
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
        glm::vec3 positionLast = glm::vec3(0);
        std::shared_ptr<Sphere> sphere;
        std::shared_ptr<SceneNode> node;
        
        void updateLightspaceMatrices();

        float updateRadius();
};

class SpotLight : public Light {
    public:
        glm::vec3 Position = glm::vec3(0);
        glm::vec3 Direction = glm::vec3(0,-1,0);

        SpotLight();

        // Methods
        void GenerateLightspaceMatrix(const float fov_deg, const float aspect, const float near, const float far);
        // Modifiers
        void SetAttenuation(float constant, float linear, float quadratic);
        // Accessors
        glm::mat4& GetLightspaceMatrix();
        float GetAttenuationConstant() const;
        float GetAttenuationLinear() const;
        float GetAttenuationQuadratic() const;

    private:
        glm::mat4 lightspaceMatrix;
        float fov = 90.0;
        float aspectRatio = 1.0;
        float attenuationConstant = 1.0;       // Default range 50
        float attenuationLinear = 0.09;        // Default range 50
        float attenuationQuadratic = 0.032;    // Default range 50

        glm::vec3 positionLast = glm::vec3(0);
        glm::vec3 directionLast = glm::vec3(0,-1,0);
        
        void updateLightspaceMatrix();
};

#endif
#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <iostream>
#include <vector>
#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <plum/scenenode.hpp>
#include <plum/shader.hpp>
#include <plum/shape.hpp>

using namespace std;

enum Light_Type {
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,
    LIGHT_SPOT
};

class Light {
    public:
        string Name, NameTemp;
        glm::vec3 Color = glm::vec3(1);
        static Shader* DefaultShader;
        
        virtual ~Light() {}

        // Accessors
        Light_Type GetType() const { return Type;     }
        float GetFarPlane()  const { return farPlane; }
        bool HasShadows()    const { return hasShadows; }       
    
        // Modifiers
        void SetName(string name) {
            Name = name;
            NameTemp = Name;
        }
        void DisableShadows() {
            hasShadows = false;
        }

    protected:
        Light_Type Type;
        bool hasShadows = false;
        float nearPlane = 0.1;
        float farPlane = 50;

        static unsigned int inline count = 0;

        Light(Light_Type type) : Type(type) {
            string typestring;
            switch (type) {
                case LIGHT_DIRECTIONAL: typestring = "DirLight"; break;
                case LIGHT_POINT:       typestring = "PointLight"; break;
                case LIGHT_SPOT:        typestring = "SpotLight"; break;
            }
            Name = typestring + to_string(count++);
            NameTemp = Name;
        }

};

Shader* Light::DefaultShader = nullptr;

class DirectionalLight : public Light {
    public:
        glm::vec3 Direction = glm::vec3(-1);
        
        DirectionalLight() : Light(LIGHT_DIRECTIONAL) {
        }
        
        // Modifiers
        void EnableShadows(const float width = 50.0f, const float height = 50.0f, const float near = 0.1f, const float far = 50.0f, const float dist = 20.0f) {
            hasShadows = true;
            projWidth = width;
            projHeight = height;
            nearPlane = near;
            farPlane = far;
            distance = dist;
            updateLightspaceMatrix();
        }

        // Accessors
        glm::mat4& GetLightspaceMatrix() { 
            if (!hasShadows) {
                cout << "Shadows are disabled for this light source!" << endl;
                exit(-1);
            }
            updateLightspaceMatrix();
            return lightspaceMatrix; 
        }

    private:
        glm::mat4 lightspaceMatrix = glm::identity<glm::mat4>();
        float projWidth = 50.0;
        float projHeight = 50.0;
        float distance = 20.0;
        glm::vec3 directionLast = glm::vec3(-1);

        void updateLightspaceMatrix() { 
            if (lightspaceMatrix == glm::identity<glm::mat4>() || Direction != directionLast) {
                glm::mat4 projection = glm::ortho(-projWidth/2, projWidth/2, -projHeight/2, projHeight/2, nearPlane, farPlane);
                glm::mat4 view; 
                if (Direction != glm::vec3(0,-1,0) && Direction != glm::vec3(0,1,0)) 
                    view = glm::lookAt(distance*-glm::normalize(Direction), glm::vec3(0), glm::vec3(0,1,0));
                else
                    view = glm::lookAt(distance*-glm::normalize(Direction), glm::vec3(0), glm::vec3(0,0,1));
                lightspaceMatrix = projection * view;
                directionLast = Direction;
            }
        }
        
};

class PointLight : public Light {
    public:
        glm::vec3 Position = glm::vec3(0);
        glm::vec3 Size = glm::vec3(0.1);

        PointLight() 
        : Light(LIGHT_POINT), sphere(Name, DefaultShader), node(shared_ptr<Sphere>(&sphere)) {
        }

        // Methods     
        void DrawDefault() {
            sphere.SetColor(Color);
            node.Position = Position;
            node.Scale = Size;
            node.Draw(*DefaultShader);
        }
        void Draw(Shader& shader) {
            sphere.SetColor(Color);
            node.Position = Position;
            node.Scale = Size;
            node.Draw(shader);
        }
        void EnableShadows(const float aspect = 1.0f, const float near = 0.1f, const float far = 75.0f) {
            hasShadows = true;
            aspectRatio = aspect;
            nearPlane = near;
            farPlane = far;
            updateLightspaceMatrices();
        }

        // Modifiers
        void SetAttenuation(float constant, float linear, float quadratic) {
            attenuationConstant = constant;
            attenuationLinear = linear;
            attenuationQuadratic = quadratic;
            updateRadius();
        }

        // Accessors
        shared_ptr<Sphere> GetShapeTemplate() {
            return make_shared<Sphere>(sphere);
        }
        shared_ptr<SceneNode> GetSceneNode() {
            return make_shared<SceneNode>(node);
        }
        vector<glm::mat4>& GetLightspaceMatrices() {
            if (!hasShadows) {
                cout << "Shadows are disabled for this light source!" << endl;
                exit(-1);
            }
            updateLightspaceMatrices();
            return lightspaceMatrices; 
        }
        float GetRadius() { return radius; }
        float GetAttenuationConstant() const { return attenuationConstant; }
        float GetAttenuationLinear() const { return attenuationLinear; }
        float GetAttenuationQuadratic() const { return attenuationQuadratic; }

    private:
        float radius = 5;
        float attenuationConstant = 1.0;       // Default range 50
        float attenuationLinear = 0.09;        // Default range 50
        float attenuationQuadratic = 0.032;    // Default range 50
        vector<glm::mat4> lightspaceMatrices;
        float aspectRatio = 1.0;
        glm::vec3 positionLast = glm::vec3(0);
        Sphere sphere;
        SceneNode node;
        
        void updateLightspaceMatrices() { 
            glm::mat4 projection = glm::perspective(glm::half_pi<float>(), aspectRatio, nearPlane, farPlane);
            if (lightspaceMatrices.empty() || Position != positionLast) {
                if (!lightspaceMatrices.empty()) {
                    lightspaceMatrices[0] = projection * glm::lookAt(Position, Position + glm::vec3(1,0,0), glm::vec3(0,-1,0));
                    lightspaceMatrices[1] = projection * glm::lookAt(Position, Position + glm::vec3(-1,0,0), glm::vec3(0,-1,0));
                    lightspaceMatrices[2] = projection * glm::lookAt(Position, Position + glm::vec3(0,1,0), glm::vec3(0,0,1));
                    lightspaceMatrices[3] = projection * glm::lookAt(Position, Position + glm::vec3(0,-1,0), glm::vec3(0,0,-1));
                    lightspaceMatrices[4] = projection * glm::lookAt(Position, Position + glm::vec3(0,0,1), glm::vec3(0,-1,0));
                    lightspaceMatrices[5] = projection * glm::lookAt(Position, Position + glm::vec3(0,0,-1), glm::vec3(0,-1,0));
                } else {
                    lightspaceMatrices.push_back( projection * glm::lookAt(Position, Position + glm::vec3(1,0,0), glm::vec3(0,-1,0))  );
                    lightspaceMatrices.push_back( projection * glm::lookAt(Position, Position + glm::vec3(-1,0,0), glm::vec3(0,-1,0)) );
                    lightspaceMatrices.push_back( projection * glm::lookAt(Position, Position + glm::vec3(0,1,0), glm::vec3(0,0,1))   );
                    lightspaceMatrices.push_back( projection * glm::lookAt(Position, Position + glm::vec3(0,-1,0), glm::vec3(0,0,-1)) );
                    lightspaceMatrices.push_back( projection * glm::lookAt(Position, Position + glm::vec3(0,0,1), glm::vec3(0,-1,0))  );
                    lightspaceMatrices.push_back( projection * glm::lookAt(Position, Position + glm::vec3(0,0,-1), glm::vec3(0,-1,0)) );
                }
                positionLast = Position;
            }
        }

        float updateRadius() {
            float lightMax = std::fmaxf(std::fmaxf(Color.r, Color.g), Color.b);
            radius = 
            (-attenuationLinear + std::sqrtf(attenuationLinear * attenuationLinear - 4 * attenuationQuadratic * (attenuationConstant - (256.0 / 5.0) * lightMax))) 
            / (2 * attenuationQuadratic);  
            return radius;
        }
};

class SpotLight : public Light {
    public:
        glm::vec3 Position = glm::vec3(0);
        glm::vec3 Direction = glm::vec3(0,-1,0);

        SpotLight() : Light(LIGHT_SPOT) {
        }

        // Methods
        void GenerateLightspaceMatrix(const float fov_deg, const float aspect, const float near, const float far) {
            nearPlane = near;
            farPlane = far;
            glm::mat4 projection = glm::perspective(glm::radians(fov_deg), aspect, near, far);
            glm::mat4 view; 
            if (Direction != glm::vec3(0,-1,0) && Direction != glm::vec3(0,1,0)) 
                view = glm::lookAt(10.f*-glm::normalize(Direction), glm::vec3(0), glm::vec3(0,1,0));
            else
                view = glm::lookAt(10.f*-glm::normalize(Direction), glm::vec3(0), glm::vec3(0,0,1));
            lightspaceMatrix = projection * view;
        }

        // Modifiers
        void SetAttenuation(float constant, float linear, float quadratic) {
            attenuationConstant = constant;
            attenuationLinear = linear;
            attenuationQuadratic = quadratic;
        }

        // Accessors
        glm::mat4& GetLightspaceMatrix() { 
            if (!hasShadows) {
                cout << "Shadows are disabled for this light source!" << endl;
                exit(-1);
            }
            updateLightspaceMatrix();
            return lightspaceMatrix; 
        }
        float GetAttenuationConstant() const  { return attenuationConstant; }
        float GetAttenuationLinear() const    { return attenuationLinear; }
        float GetAttenuationQuadratic() const { return attenuationQuadratic; }

    private:
        glm::mat4 lightspaceMatrix;
        float fov = 90.0;
        float aspectRatio = 1.0;
        float attenuationConstant = 1.0;       // Default range 50
        float attenuationLinear = 0.09;        // Default range 50
        float attenuationQuadratic = 0.032;    // Default range 50

        glm::vec3 positionLast = glm::vec3(0);
        glm::vec3 directionLast = glm::vec3(0,-1,0);
        
        void updateLightspaceMatrix() { 
            if (Direction != directionLast) {
                GenerateLightspaceMatrix(fov, aspectRatio, nearPlane, farPlane);
                directionLast = Direction;
            }
        }
};

#endif
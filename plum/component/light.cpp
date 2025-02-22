#include <plum/component/light.hpp>

#include <iostream>
#include <glad/gl.h>

namespace Component {

    float Light::GetFarPlane() const { 
        return farPlane; 
    }

    bool Light::HasShadows() const {
        return hasShadows;
    }       

    void Light::DisableShadows() {
        hasShadows = false;
    }

    Light::Light(const SceneObjectType type) : SceneObject(type) {}

    // ======== DirectionalLight ========

    DirectionalLight::DirectionalLight() : Light(SceneObjectType::DirLight) {}

    void DirectionalLight::Draw() {    }

    void DirectionalLight::Draw(const glm::mat4& parent_transf) {
        updateLightspaceMatrix();
    }

    void DirectionalLight::EnableShadows(const float width, const float height, const float near, const float far, const float dist) {
        hasShadows = true;
        projWidth = width;
        projHeight = height;
        nearPlane = near;
        farPlane = far;
        distance = dist;
        updateLightspaceMatrix();
    }

    glm::mat4& DirectionalLight::GetLightspaceMatrix() { 
        if (!hasShadows) {
            std::cout << "Shadows are disabled for this light source!" << std::endl;
            exit(-1);
        }
        updateLightspaceMatrix();
        return lightspaceMatrix; 
    }

    void DirectionalLight::updateLightspaceMatrix() { 
        if (lightspaceMatrix == glm::identity<glm::mat4>()) {
            glm::mat4 projection = glm::ortho(-projWidth/2, projWidth/2, -projHeight/2, projHeight/2, nearPlane, farPlane);
            glm::mat4 view; 
            if (direction != DOWN && direction != UP) 
                view = glm::lookAt(distance*-glm::normalize(direction), glm::vec3(0), UP);
            else
                view = glm::lookAt(distance*-glm::normalize(direction), glm::vec3(0), FRONT);
            lightspaceMatrix = projection * view;
        }
    }


    // ======== PointLight ========

    PointLight::PointLight() : Light(SceneObjectType::PointLight) {}
    
    void PointLight::Draw() {

    }

    void PointLight::Draw(const glm::mat4& parent_transf) {
        position = parent_transf[3];
        updateLightspaceMatrices();
    }

    void PointLight::EnableShadows(const float aspect, const float near, const float far) {
        hasShadows = true;
        aspectRatio = aspect;
        nearPlane = near;
        farPlane = far;
        updateLightspaceMatrices();
    }

    void PointLight::SetAttenuation(float constant, float linear, float quadratic) {
        attenuationConstant = constant;
        attenuationLinear = linear;
        attenuationQuadratic = quadratic;
        updateRadius();
    }

    std::vector<glm::mat4>& PointLight::GetLightspaceMatrices() {
        if (!hasShadows) {
            std::cout << "Shadows are disabled for this light source!" << std::endl;
            exit(-1);
        }
        updateLightspaceMatrices();
        return lightspaceMatrices; 
    }
    float PointLight::GetRadius() const { 
        return radius; 
    }
    float PointLight::GetAttenuationConstant() const { 
        return attenuationConstant; 
    }
    float PointLight::GetAttenuationLinear() const { 
        return attenuationLinear; 
    }
    float PointLight::GetAttenuationQuadratic() const { 
        return attenuationQuadratic; 
    }

    void PointLight::updateLightspaceMatrices() { 
        glm::mat4 projection = glm::perspective(glm::half_pi<float>(), aspectRatio, nearPlane, farPlane);

        if (lightspaceMatrices.empty()) {
            if (!lightspaceMatrices.empty()) {
                lightspaceMatrices[0] = projection * glm::lookAt(position, position + RIGHT,  DOWN);
                lightspaceMatrices[1] = projection * glm::lookAt(position, position + LEFT,   DOWN);
                lightspaceMatrices[2] = projection * glm::lookAt(position, position + UP,    FRONT);
                lightspaceMatrices[3] = projection * glm::lookAt(position, position + DOWN,   BACK);
                lightspaceMatrices[4] = projection * glm::lookAt(position, position + FRONT,  DOWN);
                lightspaceMatrices[5] = projection * glm::lookAt(position, position + BACK,   DOWN);
            } else {
                lightspaceMatrices.push_back( projection * glm::lookAt(position, position + RIGHT,  DOWN));
                lightspaceMatrices.push_back( projection * glm::lookAt(position, position + LEFT,   DOWN));
                lightspaceMatrices.push_back( projection * glm::lookAt(position, position + UP,    FRONT));
                lightspaceMatrices.push_back( projection * glm::lookAt(position, position + DOWN,   BACK));
                lightspaceMatrices.push_back( projection * glm::lookAt(position, position + FRONT,  DOWN));
                lightspaceMatrices.push_back( projection * glm::lookAt(position, position + BACK,   DOWN));
            }
        }
    }

    float PointLight::updateRadius() {
        float lightMax = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
        radius = 
        (-attenuationLinear + std::sqrtf(attenuationLinear * attenuationLinear - 4 * attenuationQuadratic * (attenuationConstant - (256.0 / 5.0) * lightMax))) 
        / (2 * attenuationQuadratic);  
        return radius;
    }

}
#include <plum/component/light.hpp>

#include <plum/util/transform.hpp>

#include <glad/gl.h>

#include <iostream>

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

    Light::Light(ComponentType type) 
        : ComponentBase(type) 
    {}

    // ======== DirectionalLight ========

    DirectionalLight::DirectionalLight() 
        : Light(ComponentType::DirLight) 
    {}

    void DirectionalLight::Draw(const glm::mat4& parent_transform) {
        glm::mat4 rotation = Transform::ExtractRotation(parent_transform);
        glm::vec3 direction = glm::vec3(rotation * glm::vec4(0,0,1,1));
        updateLightspaceMatrix(direction);
    }

    void DirectionalLight::EnableShadows(float width, float height, float near, float far, float dist) {
        hasShadows = true;
        projWidth = width;
        projHeight = height;
        nearPlane = near;
        farPlane = far;
        distance = dist;
    }

    glm::mat4& DirectionalLight::GetLightspaceMatrix() { 
        if (!hasShadows) {
            std::cout << "Shadows are disabled for this light source!" << std::endl;
            exit(-1);
        }
        return lightspaceMatrix; 
    }

    void DirectionalLight::updateLightspaceMatrix(glm::vec3 direction) { 
        glm::mat4 projection = glm::ortho(-projWidth/2, projWidth/2, -projHeight/2, projHeight/2, nearPlane, farPlane);
        glm::mat4 view; 
        if (direction != DOWN && direction != UP) 
            view = glm::lookAt(distance*-glm::normalize(direction), glm::vec3(0), UP);
        else
            view = glm::lookAt(distance*-glm::normalize(direction), glm::vec3(0), FRONT);
        lightspaceMatrix = projection * view;
    }

    // ======== PointLight ========

    PointLight::PointLight() 
        : Light(ComponentType::PointLight) 
    {}

    void PointLight::Draw(const glm::mat4& parent_transform) {
        updateLightspaceMatrices(parent_transform[3]);
    }

    void PointLight::EnableShadows(float aspect, float near, float far) {
        hasShadows = true;
        aspectRatio = aspect;
        nearPlane = near;
        farPlane = far;
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

    void PointLight::updateLightspaceMatrices(glm::vec3 position) { 
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
        radius = (-attenuationLinear + std::sqrtf(attenuationLinear * attenuationLinear - 4 * attenuationQuadratic * (attenuationConstant - (256.0 / 5.0) * lightMax))) 
        / (2 * attenuationQuadratic);  
        return radius;
    }

}
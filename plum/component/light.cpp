#include <plum/component/light.hpp>

#include <plum/util/direction.hpp>
#include <plum/util/transform.hpp>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include <iostream>

namespace Component {     
    
    Light::Light(ComponentType type)
        : ComponentBase(type)
    {}

    void Light::DisableShadows() {
        hasShadows = false;
    }

    void Light::DisplayWidget() {
        ImGui::ColorEdit3("Color", glm::value_ptr(color), ImGuiColorEditFlags_Float);
        ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 0.0f, "%.3f");
        ImGui::Checkbox("Cast Shadows", &hasShadows);
    }

    // ======== DirectionalLight ========

    DirectionalLight::DirectionalLight() 
        : Light(ComponentType::DirLight)
    {
        name = "Directional Light";
    }

    void DirectionalLight::Draw(const glm::mat4& parent_transform) {
        const glm::mat4 rotation = Transform::ExtractRotation(parent_transform);
        const glm::vec3 direction = glm::mat3(rotation) * glm::vec3(0,0,1);
        if (direction != lastDirection) {
            updateLightspaceMatrix(direction);
            lastDirection = direction;
        }
    }

    void DirectionalLight::EnableShadows(float width, float height, float near, float far, float dist) {
        hasShadows = true;
        projWidth = width;
        projHeight = height;
        nearPlane = near;
        farPlane = far;
        distance = dist;
    }

    const glm::mat4& DirectionalLight::LightspaceMatrix() const { 
        if (!hasShadows) {
            throw std::runtime_error("Shadows are disabled for this light source!");
        }
        return lightspaceMatrix; 
    }

    void DirectionalLight::updateLightspaceMatrix(const glm::vec3& direction) { 
        const glm::mat4 projection = glm::ortho(-projWidth/2, projWidth/2, -projHeight/2, projHeight/2, nearPlane, farPlane);
        glm::mat4 view; 
        if (direction != Direction::down && direction != Direction::up) 
            view = glm::lookAt(-distance*direction, glm::vec3(0), Direction::up);
        else
            view = glm::lookAt(-distance*direction, glm::vec3(0), Direction::front);
        lightspaceMatrix = projection * view;
    }

    // ======== PointLight ========

    PointLight::PointLight() 
        : Light(ComponentType::PointLight),
        lightspaceMatrices(6)
    {
        name = "Point Light";
    }

    void PointLight::Draw(const glm::mat4& parent_transform) {
        const glm::vec3 position = parent_transform[3];
        if (position != lastPosition) {
            updateLightspaceMatrices(position);
            lastPosition = position;
        }
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

    const std::vector<glm::mat4>& PointLight::LightspaceMatrices() const {
        if (!hasShadows) {
            throw std::runtime_error("Shadows are disabled for this light source!");
        }
        return lightspaceMatrices; 
    }

    void PointLight::updateLightspaceMatrices(const glm::vec3& position) { 
        const glm::mat4 projection = glm::perspective(glm::half_pi<float>(), aspectRatio, nearPlane, farPlane);

        lightspaceMatrices[0] = projection * glm::lookAt(position, position + Direction::right, Direction::down);
        lightspaceMatrices[1] = projection * glm::lookAt(position, position + Direction::left,  Direction::down);
        lightspaceMatrices[2] = projection * glm::lookAt(position, position + Direction::up,    Direction::front);
        lightspaceMatrices[3] = projection * glm::lookAt(position, position + Direction::down,  Direction::back);
        lightspaceMatrices[4] = projection * glm::lookAt(position, position + Direction::front, Direction::down);
        lightspaceMatrices[5] = projection * glm::lookAt(position, position + Direction::back,  Direction::down);
    }

    float PointLight::updateRadius() {
        float lightMax = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
        radius = (-attenuationLinear + std::sqrtf(attenuationLinear * attenuationLinear - 4 * attenuationQuadratic * (attenuationConstant - (256.0 / 5.0) * lightMax))) 
        / (2 * attenuationQuadratic);  
        return radius;
    }

}
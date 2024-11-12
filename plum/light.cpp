#include <plum/light.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <plum/scenenode.hpp>
#include <plum/shader.hpp>
#include <plum/shape.hpp>

// ======== Light ========
float Light::GetFarPlane() const { 
    return farPlane; 
}

bool Light::HasShadows() const {
    return hasShadows;
}       

void Light::SetName(std::string name) {
    Name = name;
    NameTemp = Name;
}
void Light::DisableShadows() {
    hasShadows = false;
}

Light::Light(std::string type) {
    Name = type + std::to_string(count++);
    NameTemp = Name;
}


// ======== DirectionalLight ========

DirectionalLight::DirectionalLight() : Light("DirLight") {}

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


// ======== PointLight ========

PointLight::PointLight() : Light("PointLight") {
    sphere = std::make_shared<Sphere>(Name, DefaultShader);
    node = std::make_shared<SceneNode>(sphere);
}

void PointLight::DrawDefault() {
    sphere->SetColor(Color);
    node->Position = Position;
    node->Scale = Size;
    node->Draw(*DefaultShader);
}
void PointLight::Draw(Shader& shader) {
    sphere->SetColor(Color);
    node->Position = Position;
    node->Scale = Size;
    node->Draw(shader);
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

std::shared_ptr<Sphere> PointLight::GetShapeTemplate() {
    return sphere;
}
std::shared_ptr<SceneNode> PointLight::GetSceneNode() {
    return node;
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

float PointLight::updateRadius() {
    float lightMax = std::fmaxf(std::fmaxf(Color.r, Color.g), Color.b);
    radius = 
    (-attenuationLinear + std::sqrtf(attenuationLinear * attenuationLinear - 4 * attenuationQuadratic * (attenuationConstant - (256.0 / 5.0) * lightMax))) 
    / (2 * attenuationQuadratic);  
    return radius;
}


// ======== SpotLight ========

SpotLight::SpotLight() : Light("SpotLight") {
}

// Methods
void SpotLight::GenerateLightspaceMatrix(const float fov_deg, const float aspect, const float near, const float far) {
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

void SpotLight::SetAttenuation(float constant, float linear, float quadratic) {
    attenuationConstant = constant;
    attenuationLinear = linear;
    attenuationQuadratic = quadratic;
}

glm::mat4& SpotLight::GetLightspaceMatrix() { 
    if (!hasShadows) {
        std::cout << "Shadows are disabled for this light source!" << std::endl;
        exit(-1);
    }
    updateLightspaceMatrix();
    return lightspaceMatrix; 
}
float SpotLight::GetAttenuationConstant() const { 
    return attenuationConstant; 
}
float SpotLight::GetAttenuationLinear() const {
    return attenuationLinear; 
}
float SpotLight::GetAttenuationQuadratic() const { 
    return attenuationQuadratic; 
}

void SpotLight::updateLightspaceMatrix() { 
    if (Direction != directionLast) {
        GenerateLightspaceMatrix(fov, aspectRatio, nearPlane, farPlane);
        directionLast = Direction;
    }
}
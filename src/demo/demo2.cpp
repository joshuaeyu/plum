#include "demo/demo2.hpp"

#include "asset/image.hpp"
#include "asset/manager.hpp"
#include "component/all.hpp"

#include <glm/glm.hpp>

#include <iostream>

Demo2::Demo2()
    : Demo("Demo 2 - Shapes")
{}

void Demo2::initialize() {
    std::clog << "Setting up skybox..." << std::endl;
    auto kloppenheim = AssetManager::Instance().LoadHot<ImageAsset>("assets/skyboxes/kloppenheim_4k.hdr");
    auto skybox = std::make_shared<Material::Texture>(kloppenheim, Material::TextureType::Diffuse);
    environment->Setup(skybox);

    std::clog << "Defining materials..." << std::endl;
    auto copper = std::make_shared<Material::PBRMetallicMaterial>();
    copper->name = "Copper";
    copper->albedo = glm::pow(glm::vec3(250, 208, 192)/255.f, glm::vec3(2.2));
    copper->metallic = 1.0;
    copper->roughness = 0.3;
    Material::materials.insert(copper);
    auto iron = std::make_shared<Material::PBRMetallicMaterial>();
    iron->name = "Iron";
    iron->albedo = glm::pow(glm::vec3(198, 198, 200)/255.f, glm::vec3(2.2));
    iron->metallic = 1.0;
    iron->roughness = 0.3;
    Material::materials.insert(iron);
    auto gold = std::make_shared<Material::PBRMetallicMaterial>();
    gold->name = "Gold";
    gold->albedo = glm::pow(glm::vec3(255, 226, 155)/255.f, glm::vec3(2.2));
    gold->metallic = 1.0;
    gold->roughness = 0.3;
    Material::materials.insert(gold);

    std::clog << "Creating components..." << std::endl;
    camera = std::make_unique<Component::Camera>();
    camera->Translate(-5,3,-0.25);
    camera->Rotate(0,-90);
    auto dirlight = std::make_shared<Component::DirectionalLight>();
    dirlight->color = glm::vec3(0.5,0.5,1.0);
    dirlight->intensity = 10.f;
    dirlight->EnableShadows();
    auto pointlight = std::make_shared<Component::PointLight>();
    pointlight->color = glm::vec3(1.0,1.0,0.5);
    pointlight->intensity = 10.f;
    pointlight->EnableShadows();
    auto plane = std::make_shared<Component::Plane>();
    plane->material = copper;
    auto sphere = std::make_shared<Component::Sphere>();
    sphere->material = gold;
    auto cube = std::make_shared<Component::Cube>();
    cube->material = iron;
    
    std::clog << "Defining scene..." << std::endl;
    scene = std::make_unique<Scene::Scene>();
    auto dlNode = scene->EmplaceChild(dirlight);
    dlNode->transform.Rotate(80,0,0);
    auto plNode = scene->EmplaceChild(pointlight);
    plNode->transform.Translate(0,5,0);
    auto planeNode = scene->EmplaceChild(plane);
    planeNode->transform.Scale(25);
    planeNode->transform.Translate(0,-5,0);
    auto sphereNode = scene->EmplaceChild(sphere);
    sphereNode->transform.Translate(0,2,0);
    auto cubeNode = sphereNode->EmplaceChild(cube);
    cubeNode->transform.Translate(0,2,0);

}

void Demo2::displayScene() {
}

void Demo2::displayGui() {
}

void Demo2::cleanUp() {
}
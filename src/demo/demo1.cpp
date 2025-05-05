#include "demo/demo1.hpp"

#include "asset/image.hpp"
#include "asset/manager.hpp"
#include "component/all.hpp"
#include "util/color.hpp"

#include <glm/glm.hpp>

#include <iostream>

Demo1::Demo1()
    : Demo("Demo 1 - Sponza")
{}

void Demo1::initialize() {
    std::clog << "Setting up skybox..." << std::endl;
    auto rogland = AssetManager::Instance().LoadHot<ImageAsset>("assets/skyboxes/rogland_clear_night_4k.hdr");
    auto skybox = std::make_shared<Material::Texture>(rogland, Material::TextureType::Diffuse);
    environment->Setup(skybox);

    std::clog << "Creating components..." << std::endl;
    camera = std::make_unique<Component::Camera>();
    camera->Translate(-5,3,-0.25);
    camera->Rotate(15,-90);
    auto pointlight = std::make_shared<Component::PointLight>();
    pointlight->color = Color::white;
    pointlight->intensity = 10.f;
    pointlight->EnableShadows();
    
    std::clog << "Loading models..." << std::endl;
    auto sponzaAsset = AssetManager::Instance().LoadHot<ModelAsset>("assets/models/sponza/glTF/Sponza.gltf");
    auto sponza = std::make_shared<Component::Model>(sponzaAsset);
    
    std::clog << "Defining scene..." << std::endl;
    scene = std::make_unique<Scene::Scene>();
    auto plNode = scene->EmplaceChild(pointlight);
    plNode->transform.Translate(0,3,0);
    auto sponzaNode = scene->EmplaceChild(sponza);
    sponzaNode->name = "Sponza";
}

void Demo1::preDisplayScene() {
}

void Demo1::postDisplayScene() {
}

void Demo1::displayGui() {
}

void Demo1::cleanUp() {
}
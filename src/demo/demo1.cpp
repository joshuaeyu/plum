#include "demo1.hpp"

#include "component/all.hpp"
#include "asset/image.hpp"
#include "asset/manager.hpp"
#include "util/time.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>

Demo1::Demo1()
    : Demo("Demo1")
{}

void Demo1::Initialize() {
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
    
    std::clog << "Setting up skybox..." << std::endl;
    auto kloppenheim = AssetManager::Instance().LoadHot<ImageAsset>("assets/skyboxes/kloppenheim_4k.hdr");
    auto skybox = std::make_shared<Material::Texture>(kloppenheim, Material::TextureType::Diffuse);
    environment->Setup(skybox);

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
    
    std::clog << "Loading models..." << std::endl;
    // auto backpack = std::make_shared<Component::Model>("assets/models/survival_guitar_backpack/scene.gltf", 0.005f);
    // models["Backpack"] = backpack;
    // auto sponzaAsset = AssetManager::Instance().LoadHot<ModelAsset>("assets/models/sponza/glTF/Sponza.gltf");
    // auto sponza = std::make_shared<Component::Model>(sponzaAsset);
    
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
    // auto backpackNode = scene->EmplaceChild(backpack);
    // backpackNode->transform.Translate(5,4,0);
    // auto sponzaNode = scene->EmplaceChild(sponza);
    // sponzaNode->name = "Sponza";
}

void Demo1::Display() {
    camera->ProcessInputs();
    
    // backpackNode->transform.Rotate(glm::vec3(0,30,0) * app.DeltaTime());
    // cubeNode->transform.Rotate(glm::vec3(50,120,90) * app.DeltaTime());

    environment->iblIntensity = renderOptions.iblIntensity;
    renderer->ssao = renderOptions.ssao;

    static Core::Fbo* fbo;
    fbo = renderer->Render(*scene, *camera, *environment);
    if (renderOptions.bloom) {
        fbo = bloom->Process(*fbo);
    }
    if (renderOptions.hdr) {
        hdr->exposure = renderOptions.hdrExposure;
        fbo = hdr->Process(*fbo);
    }
    if (renderOptions.fxaa) {
        fbo = fxaa->Process(*fbo);
    }
    fbo->BlitToDefault();

    displayMainGui();
}
void Demo1::CleanUp() {
    scene.reset();
    environment.reset();
    camera.reset();
    renderer.reset();
    fxaa.reset();
    hdr.reset();
    bloom.reset();
}
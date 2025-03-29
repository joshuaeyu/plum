#include <plum/context/context.hpp>
#include <plum/context/window.hpp>
#include <plum/scene/scene.hpp>
#include <plum/scene/environment.hpp>
#include <plum/component/camera.hpp>
#include <plum/component/primitive.hpp>
#include <plum/component/model.hpp>
#include <plum/renderer/renderer.hpp>
#include <plum/renderer/postprocessing.hpp>
#include <plum/material/texture.hpp>

#include <plum/core/program.hpp>
#include <plum/material/material.hpp>

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>

int main() {
    std::cout << "Initializing app..." << std::endl;
    Context::Application& app = Context::Application::Instance();
    
    std::cout << "Setting window parameters..." << std::endl;
    app.defaultWindow->SetTitle("Plum Engine v2.0");
    app.defaultWindow->SetWindowSize(1200,800);
    
    std::cout << "Setting up environment..." << std::endl;
    // auto skybox = std::make_shared<Material::Texture>("assets/textures/black.png", Material::TextureType::Diffuse);
    // auto skybox = std::make_shared<Material::Texture>("assets/textures/dresden_station_4k.hdr", Material::TextureType::Diffuse);
    auto skybox = std::make_shared<Material::Texture>("assets/textures/kloppenheim_4k.hdr", Material::TextureType::Diffuse);
    // static std::vector<std::string> oceanSkyboxPaths = {
    //     "assets/textures/skybox/right.jpg",
    //     "assets/textures/skybox/left.jpg",
    //     "assets/textures/skybox/top.jpg",
    //     "assets/textures/skybox/bottom.jpg",
    //     "assets/textures/skybox/front.jpg",
    //     "assets/textures/skybox/back.jpg"
    // };
    // static auto skybox = std::make_shared<Material::Texture>(oceanSkyboxPaths, Material::TextureType::Diffuse, false);
    Scene::Environment environment(skybox->tex);
    
    std::cout << "Defining materials..." << std::endl;
    auto copper = std::make_shared<Material::PBRMetallicMaterial>();
    copper->albedo = glm::pow(glm::vec3(0.72,0.45,0.22),glm::vec3(2.2));
    copper->metallic = 1.0;
    copper->roughness = 0.15;
    auto ruby = std::make_shared<Material::PBRMetallicMaterial>();
    ruby->albedo = glm::pow(glm::vec3(0.6,0.1,0.1),glm::vec3(2.2));
    ruby->metallic = 0.0;
    ruby->roughness = 0.1;
    auto sapphire = std::make_shared<Material::PBRMetallicMaterial>();
    sapphire->albedo = glm::pow(glm::vec3(0.1,0.2,0.7),glm::vec3(2.2));
    sapphire->metallic = 1.0;
    sapphire->roughness = 0.2;

    std::cout << "Creating components..." << std::endl;
    Component::Camera camera;
    camera.transform.Translate(0,3,-5);
    auto dirlight = std::make_shared<Component::DirectionalLight>();
    dirlight->color = glm::vec3(0.5,0.5,1.0);
    dirlight->intensity = 10.f;
    dirlight->EnableShadows();
    auto pointlight = std::make_shared<Component::PointLight>();
    pointlight->color = glm::vec3(1.0,1.0,0.5);
    pointlight->intensity = 25.f;
    pointlight->EnableShadows();
    auto plane = std::make_shared<Component::Plane>();
    plane->material = copper;
    auto sphere = std::make_shared<Component::Sphere>();
    sphere->material = sapphire;
    auto cube = std::make_shared<Component::Cube>();
    cube->material = ruby;

    std::cout << "Loading models..." << std::endl;
    auto backpack = std::make_shared<Component::Model>("assets/models/survival_guitar_backpack/scene.gltf", 0.005f);
    auto sponza = std::make_shared<Component::Model>("assets/models/sponza/glTF/Sponza.gltf");

    std::cout << "Defining scene..." << std::endl;
    Scene::Scene scene;
    auto dlNode = scene.AddChild(dirlight);
    dlNode->transform.Rotate(45,0,0);
    auto plNode = scene.AddChild(pointlight);
    plNode->transform.Translate(0,5,0);
    auto planeNode = scene.AddChild(plane);
    planeNode->transform.Scale(25);
    planeNode->transform.Translate(0,-5,0);
    auto cubeNode = scene.AddChild(cube);
    cubeNode->transform.Translate(0,2,0);
    auto sphereNode = cubeNode->AddChild(sphere);
    sphereNode->transform.Translate(0,2,0);
    auto backpackNode = scene.AddChild(backpack);
    backpackNode->transform.Translate(5,4,0);
    auto sponzaNode = scene.AddChild(sponza);
    
    std::cout << "Creating renderer..." << std::endl;
    Renderer::DeferredRenderer renderer(app.defaultWindow);
    auto fxaa = PostProcessing::Fxaa();
    auto hdr = PostProcessing::Hdr();
    auto bloom = PostProcessing::Bloom();
    
    // Put this in some MainLoop() function
    std::cout << "Starting main loop..." << std::endl;
    while (!app.defaultWindow->ShouldClose()) {
        
        // Pre display
        app.PollInputsAndEvents();    // needed!
        camera.ProcessInputs(); // needed because camera uses an inputobserver every frame
        
        // Display
        backpackNode->transform.Rotate(glm::vec3(0,30,0) * app.DeltaTime());
        cubeNode->transform.Rotate(glm::vec3(50,120,90) * app.DeltaTime());

        Core::Fbo* fbo;
        fbo = renderer.Render(scene, camera, environment);
        if (glfwGetKey(app.activeWindow->Handle(), GLFW_KEY_1) == GLFW_PRESS)
            fbo = bloom.Process(*fbo);
        if (glfwGetKey(app.activeWindow->Handle(), GLFW_KEY_2) != GLFW_PRESS)
            fbo = hdr.Process(*fbo);
        if (glfwGetKey(app.activeWindow->Handle(), GLFW_KEY_0) != GLFW_PRESS)
            fbo = fxaa.Process(*fbo);
        fbo->BlitToDefault();
        while (GLenum error = glGetError()) { std::cerr << "Render error: " << error << std::endl; }

        // Post display?
        app.defaultWindow->SwapBuffers();

    }
}
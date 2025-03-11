#include <plum/context/context.hpp>
#include <plum/context/window.hpp>
#include <plum/scene/scene.hpp>
#include <plum/scene/environment.hpp>
#include <plum/component/camera.hpp>
#include <plum/component/primitive.hpp>
#include <plum/renderer/renderer.hpp>
#include <plum/material/texture.hpp>

#include <plum/core/program.hpp>
#include <plum/material/material.hpp>

#include <iostream>
#include <vector>
#include <string>

int main() {
    std::cout << "Initializing app..." << std::endl;
    Context::Application& app = Context::Application::Instance();
    
    std::cout << "Setting window parameters..." << std::endl;
    app.defaultWindow->SetTitle("Woohoo!");
    app.defaultWindow->SetWindowSize(1024,1024);
    
    std::cout << "Setting up environment..." << std::endl;
    auto skybox = std::make_shared<Material::Texture>("assets/textures/dresden_station_4k.hdr");
    static std::vector<std::string> oceanSkyboxPaths = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg"
    };
    static auto skybox2 = std::make_shared<Material::Texture>(oceanSkyboxPaths, false);
    Scene::Environment environment(skybox->tex);
    
    std::cout << "Creating components..." << std::endl;
    Component::Camera camera;
    camera.projection = glm::perspective(45.f, 1024.f/1024.f, 0.1f, 100.f);
    camera.transform.Translate(0,3,-5);
    auto dirlight = std::make_shared<Component::DirectionalLight>();
    dirlight->color = glm::vec3(0.5,0.5,1.0);
    dirlight->intensity = 0.f;
    dirlight->EnableShadows();
    auto pointlight = std::make_shared<Component::PointLight>();
    pointlight->color = glm::vec3(1.0,1.0,0.5);
    pointlight->intensity = 0.f;
    pointlight->EnableShadows();
    auto plane = std::make_shared<Component::Plane>();
    auto sphere = std::make_shared<Component::Sphere>();
    auto cube = std::make_shared<Component::Cube>();
    
    std::cout << "Defining scene..." << std::endl;
    Scene::Scene scene;
    auto dlNode = scene.AddChild(dirlight);
    dlNode->transform.Rotate(45,0,0);
    auto plNode = scene.AddChild(pointlight);
    plNode->transform.Translate(0,10,0);
    auto planeNode = scene.AddChild(plane);
    planeNode->transform.Scale(25);
    auto cubeNode = scene.AddChild(cube);
    cubeNode->transform.Translate(0,2,0);
    auto sphereNode = cubeNode->AddChild(sphere);
    sphereNode->transform.Translate(0,2,0);
    
    std::cout << "Creating renderer..." << std::endl;
    Renderer::DeferredRenderer renderer(app.defaultWindow);

    // Put this in some MainLoop() function
    std::cout << "Starting main loop..." << std::endl;
    while (!app.defaultWindow->ShouldClose()) {
        
        app.PollInputsAndEvents();    // needed!
        camera.ProcessInputs(); // needed because camera uses an inputobserver every frame
        
        cubeNode->transform.Rotate(0, 90*app.DeltaTime(), 50*app.DeltaTime());

        Core::Fbo* fbo = renderer.Render(scene, camera, environment);
        while (GLenum error = glGetError()) { std::cerr << "Render error: " << error << std::endl; }
        fbo->BlitToDefault();

        app.defaultWindow->SwapBuffers();

    }
}
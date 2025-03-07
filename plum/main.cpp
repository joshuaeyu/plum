#include <plum/context/context.hpp>
#include <plum/context/window.hpp>
#include <plum/scene/scene.hpp>
#include <plum/scene/environment.hpp>
#include <plum/component/camera.hpp>
#include <plum/component/primitive.hpp>
#include <plum/renderer/renderer.hpp>

#include <iostream>

int main() {
    std::cout << "Initializing application" << std::endl;
    Context::Application application = Context::Application::Instance();
    
    std::cout << "Setting window parameters..." << std::endl;
    application.defaultWindow->SetTitle("Woohoo!");
    application.defaultWindow->SetWindowSize(1024,1024);
    
    std::cout << "Creating scene..." << std::endl;
    Component::Camera camera;
    Scene::Environment environment;
    // Scene::Scene scene;
    Component::Sphere sphere;
    // scene.AddChild(sphere);
    
    // std::cout << "Creating renderer..." << std::endl;
    // Renderer::DeferredRenderer renderer(*application.defaultWindow);
    
    // Put this in some MainLoop() function
    std::cout << "Starting main loop..." << std::endl;
    while (!application.defaultWindow->ShouldClose()) {
        application.defaultWindow->PollEvents();    // needed!
        camera.ProcessInputs(); // needed because camera uses an inputobserver every frame
        // Core::Fbo output = renderer.Render(scene, camera, environment);
        // output.BlitToDefault(); // or window.display()? - need some coupling between fbo and window
        application.defaultWindow->SwapBuffers();
    }
}
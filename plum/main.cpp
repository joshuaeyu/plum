
#include <plum/context/window.hpp>
#include <plum/scene/scene.hpp>
#include <plum/component/primitive.hpp>
#include <plum/component/camera.hpp>
#include <plum/renderer/renderer.hpp>

int main() {
    Context::WindowCreator windowCreator;
    windowCreator.width = 1920;
    windowCreator.height = 1080;
    Context::Window window = windowCreator.Create();

    Scene::Scene scene;
    Component::Sphere sphere;
    scene.AddChild(sphere);

    Component::Camera camera;
    Scene::Environment environment;
    Renderer::DeferredRenderer renderer(window);
    
    // Put this in some MainLoop() function
    while (!window.ShouldClose()) {
        camera.ProcessInputs(); // needed because camera uses an inputobserver every frame
        Core::Fbo output = renderer.Render(scene, camera, environment);
        output.BlitToDefault(); // or window.display()? - need some coupling between fbo and window
        window.SwapBuffers();
    }
}
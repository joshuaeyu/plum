#include "demo/demo2.hpp"

#include "asset/image.hpp"
#include "asset/manager.hpp"
#include "component/all.hpp"

#include <glm/glm.hpp>

#include <array>
#include <iostream>
#include <random>

Demo2::Demo2()
    : Demo("Demo 2 - Shapes")
{}

void Demo2::initialize() {
    std::clog << "Setting up skybox..." << std::endl;
    auto kloppenheim = AssetManager::Instance().LoadHot<ImageAsset>("assets/skyboxes/kloppenheim_4k.hdr");
    auto skybox = std::make_shared<Material::Texture>(kloppenheim, Material::TextureType::Diffuse);
    environment->Setup(skybox);

    std::clog << "Defining materials..." << std::endl;
    auto vinylplank_albedo = std::make_shared<Material::Texture>(std::make_shared<ImageAsset>("assets/textures/luxury-vinyl-plank-bl/luxury-vinyl-plank_albedo.png"), Material::TextureType::Diffuse, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
    auto vinylplank_ao = std::make_shared<Material::Texture>(std::make_shared<ImageAsset>("assets/textures/luxury-vinyl-plank-bl/luxury-vinyl-plank_ao.png"), Material::TextureType::Occlusion, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
    auto vinylplank_normal = std::make_shared<Material::Texture>(std::make_shared<ImageAsset>("assets/textures/luxury-vinyl-plank-bl/luxury-vinyl-plank_normal-ogl.png"), Material::TextureType::Normal, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
    auto vinylplank_metallic = std::make_shared<Material::Texture>(std::make_shared<ImageAsset>("assets/textures/luxury-vinyl-plank-bl/luxury-vinyl-plank_metallic.png"), Material::TextureType::Metalness, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
    auto vinylplank_roughness = std::make_shared<Material::Texture>(std::make_shared<ImageAsset>("assets/textures/luxury-vinyl-plank-bl/luxury-vinyl-plank_roughness.png"), Material::TextureType::Roughness, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
    auto vinylplank = std::make_shared<Material::PBRMetallicMaterial>();
    vinylplank->albedo = glm::vec3(0);
    vinylplank->albedoMap = vinylplank_albedo->tex;
    vinylplank->occlusionMap = vinylplank_ao->tex;
    vinylplank->normalMap = vinylplank_normal->tex;
    vinylplank->metallicMap = vinylplank_metallic->tex;
    vinylplank->roughnessMap = vinylplank_roughness->tex;
    Material::materials.insert(vinylplank);
    
    auto copper = std::make_shared<Material::PBRMetallicMaterial>();
    copper->name = "Copper";
    copper->albedo = glm::pow(glm::vec3(250, 208, 192)/255.f, glm::vec3(2.2f));
    copper->metallic = 1.0f;
    copper->roughness = 0.45f;
    Material::materials.insert(copper);
    auto iron = std::make_shared<Material::PBRMetallicMaterial>();
    iron->name = "Iron";
    iron->albedo = glm::pow(glm::vec3(198, 198, 200)/255.f, glm::vec3(2.2f));
    iron->metallic = 1.0f;
    iron->roughness = 0.45f;
    Material::materials.insert(iron);
    auto gold = std::make_shared<Material::PBRMetallicMaterial>();
    gold->name = "Gold";
    gold->albedo = glm::pow(glm::vec3(255, 226, 155)/255.f, glm::vec3(2.2f));
    gold->metallic = 1.0f;
    gold->roughness = 0.45f;
    Material::materials.insert(gold);

    const float neonIntensity = 10.f;
    auto neonwhite = std::make_shared<Material::PBRMetallicMaterial>();
    neonwhite->name = "Neon White";
    neonwhite->albedo = Color::white * neonIntensity;
    neonwhite->metallic = 0.0f;
    neonwhite->roughness = 1.0f;
    Material::materials.insert(neonwhite);
    auto neoncyan = std::make_shared<Material::PBRMetallicMaterial>();
    neoncyan->name = "Neon Cyan";
    neoncyan->albedo = Color::cyan * neonIntensity;
    neoncyan->metallic = 0.0f;
    neoncyan->roughness = 1.0f;
    Material::materials.insert(neoncyan);
    auto neonmagenta = std::make_shared<Material::PBRMetallicMaterial>();
    neonmagenta->name = "Neon Magenta";
    neonmagenta->albedo = Color::magenta * neonIntensity;
    neonmagenta->metallic = 0.0f;
    neonmagenta->roughness = 1.0f;
    Material::materials.insert(neonmagenta);
    auto neonyellow = std::make_shared<Material::PBRMetallicMaterial>();
    neonyellow->name = "Neon Yellow";
    neonyellow->albedo = Color::yellow * neonIntensity;
    neonyellow->metallic = 0.0f;
    neonyellow->roughness = 1.0f;
    Material::materials.insert(neonyellow);

    std::clog << "Creating components..." << std::endl;
    camera = std::make_unique<Component::Camera>();
    camera->Translate(-5,3,-0.25);
    camera->Rotate(0,-90);
    
    std::clog << "Defining scene..." << std::endl;
    scene = std::make_unique<Scene::Scene>();

    // Floating cubes/spheres and lights
    std::array<std::shared_ptr<Material::PBRMetallicMaterial>, 3> metalmats = {copper, iron, gold};
    std::array<std::shared_ptr<Material::PBRMetallicMaterial>, 4> lightmats = {neonwhite, neoncyan, neonmagenta, neonyellow};
    
    std::default_random_engine eng(3);
    std::uniform_int_distribution<> intdist_shapes(0, 1);  // sphere, cube
    std::uniform_int_distribution<> intdist_metalmats(0, metalmats.size() - 1);
    const float range = 20.f;
    std::uniform_real_distribution<float> realtrans(-range/2, range/2);
    std::uniform_real_distribution<float> realrot(-90.f, 90.f);
    std::uniform_real_distribution<float> realscale(0.5f, 2.0f);
    
    auto metalParent = scene->EmplaceChild();
    metalParent->name = "Metallic Primitives";
    for (int i = 0; i < 50; i++) {
        int j = intdist_shapes(eng);
        int k = intdist_metalmats(eng);
        
        auto metalNode = metalParent->EmplaceChild();
        metalNode->transform.Translate(realtrans(eng), realtrans(eng), realtrans(eng));
        metalNode->transform.Rotate(realrot(eng), realrot(eng), realrot(eng));
        metalNode->transform.Scale(realscale(eng), realscale(eng), realscale(eng));
        metalNode->name = std::string("Primitive ") + std::to_string(i);
        
        std::shared_ptr<Component::Mesh> mesh;
        if (j == 0) {
            mesh = std::make_shared<Component::Sphere>();
        } else {
            mesh = std::make_shared<Component::Cube>();
        }
        mesh->material = metalmats[k];
        metalNode->component = mesh;
    }
    auto lightParent = scene->EmplaceChild();
    lightParent->name = "Neon Lights";
    for (int i = 0; i < 8; i++) {
        int j = intdist_shapes(eng);
        int k =  i % lightmats.size();
        
        auto lightNode = lightParent->EmplaceChild();
        lightNode->name = std::string("Light ") + std::to_string(i);
        lightNode->transform.Translate(realtrans(eng), realtrans(eng), realtrans(eng));
        auto light = std::make_shared<Component::PointLight>();
        light->color = lightmats[k]->albedo / neonIntensity;
        light->intensity = neonIntensity / 2.f;
        light->EnableShadows();
        lightNode->component = light;
        
        auto lightMeshNode = lightNode->EmplaceChild();
        lightMeshNode->transform.Scale(0.5f);
        std::shared_ptr<Component::Mesh> mesh;
        if (j == 0) {
            mesh = std::make_shared<Component::Sphere>();
        } else {
            mesh = std::make_shared<Component::Cube>();
        }
        mesh->material = lightmats[k];
        mesh->castShadows = false;
        lightMeshNode->component = mesh;

        // Turn on Bloom
        renderOptions.bloom = true;
    }
    
    // Walls
    auto wallsParent = scene->EmplaceChild();
    wallsParent->name = "Walls";
    wallsParent->transform.Scale(range);
    auto wallMesh = std::make_shared<Component::Plane>();
    wallMesh->material = vinylplank;
    auto wall_right = wallsParent->EmplaceChild(wallMesh); // Deep copy
    wall_right->name = "Right Wall";
    wall_right->transform.Translate(0.5f, 0, 0);
    wall_right->transform.Rotate(0, 0, 90);
    auto wall_left = wallsParent->EmplaceChild(wallMesh);
    wall_left->name = "Left Wall";
    wall_left->transform.Translate(-0.5f, 0, 0);
    wall_left->transform.Rotate(0, 0, -90);
    auto wall_up = wallsParent->EmplaceChild(wallMesh);
    wall_up->name = "Up Wall";
    wall_up->transform.Translate(0, 0.5f, 0);
    wall_up->transform.Rotate(180, 0, 0);
    auto wall_down = wallsParent->EmplaceChild(wallMesh);
    wall_down->name = "Down Wall";
    wall_down->transform.Translate(0, -0.5f, 0);
    wall_down->transform.Rotate(0, 0, 0);
    auto wall_front = wallsParent->EmplaceChild(wallMesh);
    wall_front->name = "Front Wall";
    wall_front->transform.Translate(0, 0, 0.5f);
    wall_front->transform.Rotate(-90, 0, 0);
    auto wall_back = wallsParent->EmplaceChild(wallMesh);
    wall_back->name = "Back Wall";
    wall_back->transform.Translate(0, 0, -0.5f);
    wall_back->transform.Rotate(90, 0, 0);
}

void Demo2::preDisplayScene() {
}

void Demo2::postDisplayScene() {
}

void Demo2::displayGui() {
}

void Demo2::cleanUp() {
    Material::materials.clear();
    Material::materials.insert(Material::defaultMaterial);
}
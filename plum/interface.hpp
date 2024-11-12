#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <map>
#include <string>

#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <plum/texture.hpp>
#include <plum/shape.hpp>

class Scene;

namespace Interface {
    struct NodeProperties {
        static int count;
        std::string name = "node" + std::to_string(NodeProperties::count);
        // Placement
        glm::vec3 scale = glm::vec3(1);
        glm::vec3 position = glm::vec3(0);
        glm::vec3 rotation = glm::vec3(0);
        // Misc
        bool bypasslighting = false;
    };
    int NodeProperties::count = 0;
    
    struct ShapeProperties {
        static int count;
        std::string name = "shape" + std::to_string(ShapeProperties::count);
        // Appearance
        glm::vec3 albedo = glm::vec3(0.5);
        float roughness = 0.5;
        float metallic = 0;
        std::map<Tex::Tex_Type, std::string> textures;
        Shape::Shape_Surface surfacetype;
        // Shape specific settings
        int stacks = 30, sectors = 30;
        int x_sections = 1, z_sections = 1;
    };
    int ShapeProperties::count = 0;

    struct LightProperties {
        static int count;
        std::string name = "light" + std::to_string(LightProperties::count);
        // Placement
        glm::vec3 position = glm::vec3(0);
        glm::vec3 size = glm::vec3(0.2);
        glm::vec3 direction = glm::vec3(-1);
        // Appearance
        glm::vec3 color = glm::vec3(0.5);
        bool shadows = true;
    };
    int LightProperties::count = 0;

    Scene* MainScene;
    std::vector<float> FramerateData;
    
    // Render options
    float Ibl = 0.1;
    bool Ssao = true;
    bool Bloom = true;
    bool Hdr = true;
    float HdrExposure = 1.0;
    bool Fxaa = true;

    void ShowInterface();

    void SetStyle(ImGuiStyle& style);

}

#endif
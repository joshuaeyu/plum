#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <map>
#include <string>

#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <plum/texture.hpp>
#include <plum/shape.hpp>

class Scene;
class Engine;
class Resources;

class Interface {
    
    public:

        Scene* MainScene;
        std::vector<float> FramerateData;
        
        // Render options
        float Ibl = 0.1;
        bool Ssao = true;
        bool Bloom = true;
        bool Hdr = true;
        float HdrExposure = 1.0;
        bool Fxaa = true;

        Interface(std::shared_ptr<Engine> w, std::shared_ptr<Resources> e);

        void ShowInterface();

        void SetStyle(ImGuiStyle& style);

    private:
        struct NodeProperties {
            NodeProperties(int count) {
                name = "node" + std::to_string(count);
            }
            std::string name;
            // Placement
            glm::vec3 scale = glm::vec3(1);
            glm::vec3 position = glm::vec3(0);
            glm::vec3 rotation = glm::vec3(0);
            // Misc
            bool bypasslighting = false;
        };
        
        struct ShapeProperties {
            ShapeProperties(int count) {
                name = "shape" + std::to_string(count);
            }
            std::string name;
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

        struct LightProperties {
            LightProperties(int count) {
                name = "light" + std::to_string(count);
            }
            std::string name;
            // Placement
            glm::vec3 position = glm::vec3(0);
            glm::vec3 size = glm::vec3(0.2);
            glm::vec3 direction = glm::vec3(-1);
            // Appearance
            glm::vec3 color = glm::vec3(0.5);
            bool shadows = true;
        };
        int nodeCount = 0;
        int shapeCount = 0;
        int lightCount = 0;

        std::shared_ptr<Engine> engine;
        std::shared_ptr<Resources> resources;

};

#endif
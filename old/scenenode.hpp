#ifndef SCENENODE_HPP
#define SCENENODE_HPP

#include <string>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

class Shader;

class Drawable {
    public:
        std::string Name, NameTemp;
        Shader* DefaultShader = nullptr;
        bool FlipYZ = false;
        
        virtual ~Drawable() {}
        virtual void Draw(Shader& shader, glm::mat4 model_matrix = glm::identity<glm::mat4>()) = 0;
    protected:
        Drawable(std::string name, Shader* default_shader, bool flip_yz = false) : Name(name), NameTemp(name), DefaultShader(default_shader), FlipYZ(flip_yz) {}
};

class SceneNode {
    public:
            
        std::string Name, NameTemp;
        // Placement
        glm::vec3 Position = glm::vec3(0);
        glm::vec3 Scale = glm::vec3(1);
        glm::vec3 Rotation = glm::vec3(0);
        
        bool BypassLighting = false;
        Shader* InstanceShader = nullptr;
        std::shared_ptr<Drawable> NodeTemplate;

        SceneNode(std::shared_ptr<Drawable> node_template);

        // Methods
        void DrawDefault();
        void Draw();
        void Draw(Shader& shader);

        // Modifiers
        void SetShader(Shader* shader);
        void SetPlacement(glm::vec3 position, glm::vec3 scale = glm::vec3(1), glm::vec3 rotation = glm::vec3(0));
        void SetName(std::string instance_name);

        // Accessors
        glm::mat4 GetModelMatrix();
    
    protected:

        static unsigned int inline count = 0;
        glm::mat4 modelMatrix = glm::mat4(1);
        glm::vec3 positionLast = glm::vec3(0);
        glm::vec3 scaleLast = glm::vec3(1);
        glm::vec3 rotationLast = glm::vec3(0);
        // shared_ptr<PointLight> linkedPointLight;
        
        void updateModelMatrix();
};

#endif
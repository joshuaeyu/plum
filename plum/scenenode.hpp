#ifndef SCENENODE_HPP
#define SCENENODE_HPP

#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <plum/shader.hpp>

class Drawable {
    public:
        string Name, NameTemp;
        Shader* DefaultShader = nullptr;
        bool FlipYZ = false;
        
        virtual ~Drawable() {}
        virtual void Draw(Shader& shader, glm::mat4 model_matrix = glm::identity<glm::mat4>()) = 0;
    protected:
        Drawable(string name, Shader* default_shader, bool flip_yz = false) : Name(name), NameTemp(name), DefaultShader(default_shader), FlipYZ(flip_yz) {}
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
        shared_ptr<Drawable> NodeTemplate;

        SceneNode(shared_ptr<Drawable> node_template) {
            NodeTemplate = node_template;
            Name = "SceneNode" + to_string(count++);
            NameTemp = Name;
        }

        // Methods
        void DrawDefault() {
            if (!NodeTemplate->DefaultShader) {
                cout << "ERROR: Cannot draw model '" << Name << "'. No default model shader is set." << endl;
                exit(-1);
            }
            Draw(*NodeTemplate->DefaultShader);
        }
        void Draw() {
            if (!InstanceShader) {
                cout << "ERROR: Cannot draw '" << Name << "'. No node shader is set." << endl;
                exit(-1);
            }
            Draw(*InstanceShader);
        }
        void Draw(Shader& shader) {
            glUseProgram(shader.programID);
            updateModelMatrix();
            NodeTemplate->Draw(shader, modelMatrix);
        }

        // Modifiers
        void SetShader(Shader* shader) {
            InstanceShader = shader;
        }
        void SetPlacement(glm::vec3 position, glm::vec3 scale = glm::vec3(1), glm::vec3 rotation = glm::vec3(0)) {
            Position = position;
            Rotation = rotation;
            Scale = scale;
        }
        void SetName(std::string instance_name) {
            Name = instance_name;
            NameTemp = Name;
        }

        // Accessors
        glm::mat4 GetModelMatrix() {
            return modelMatrix;
        }
    
    protected:

        static unsigned int inline count = 0;
        glm::mat4 modelMatrix = glm::mat4(1);
        glm::vec3 positionLast = glm::vec3(0);
        glm::vec3 scaleLast = glm::vec3(1);
        glm::vec3 rotationLast = glm::vec3(0);
        // shared_ptr<PointLight> linkedPointLight;
        
        void updateModelMatrix() {
            if (Scale != scaleLast || Position != positionLast || Rotation != rotationLast ) {
                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, Position);
                modelMatrix = glm::rotate(modelMatrix, glm::radians(Rotation.x), glm::vec3(1,0,0));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(Rotation.y), glm::vec3(0,1,0));
                modelMatrix = glm::rotate(modelMatrix, glm::radians(Rotation.z), glm::vec3(0,0,1));
                modelMatrix = glm::scale(modelMatrix, Scale);
                scaleLast = Scale;
                positionLast = Position;
                rotationLast = Rotation;
            }
        }
};

#endif
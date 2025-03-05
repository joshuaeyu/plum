#include <plum/scenenode.hpp>

#include <string>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <plum/shader.hpp>


SceneNode::SceneNode(std::shared_ptr<Drawable> node_template) {
    NodeTemplate = node_template;
    Name = "SceneNode" + std::to_string(count++);
    NameTemp = Name;
}

// Methods
void SceneNode::DrawDefault() {
    if (!NodeTemplate->DefaultShader) {
        std::cout << "ERROR: Cannot draw model '" << Name << "'. No default model shader is set." << std::endl;
        exit(-1);
    }
    Draw(*NodeTemplate->DefaultShader);
}
void SceneNode::Draw() {
    if (!InstanceShader) {
        std::cout << "ERROR: Cannot draw '" << Name << "'. No node shader is set." << std::endl;
        exit(-1);
    }
    Draw(*InstanceShader);
}
void SceneNode::Draw(Shader& shader) {
    glUseProgram(shader.programID);
    updateModelMatrix();
    NodeTemplate->Draw(shader, modelMatrix);
}

// Modifiers
void SceneNode::SetShader(Shader* shader) {
    InstanceShader = shader;
}
void SceneNode::SetPlacement(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation) {
    Position = position;
    Rotation = rotation;
    Scale = scale;
}
void SceneNode::SetName(std::string instance_name) {
    Name = instance_name;
    NameTemp = Name;
}

// Accessors
glm::mat4 SceneNode::GetModelMatrix() {
    return modelMatrix;
}


void SceneNode::updateModelMatrix() {
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

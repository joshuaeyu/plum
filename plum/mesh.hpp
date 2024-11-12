#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <plum/vertex.hpp>

class Shader;
class Tex;


class Mesh {
    public:       

        struct MeshColor {
            glm::vec3 Value;
            std::string type; // diffuse, specular, etc.
        };
        struct MeshScalar {
            float Value;
            std::string type;
        };

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Tex>> textures, std::vector<MeshColor> colors, std::vector<MeshScalar> scalars, glm::mat4 model_transformation);
        ~Mesh();

        void Draw(Shader& shader, glm::mat4 scene_model_matrix);

        void DrawInstanced(Shader& shader, GLsizei instancecount);

    private:
        std::vector<Vertex>                 vertices;
        std::vector<unsigned int>           indices;
        std::vector<std::shared_ptr<Tex>>   textures;
        std::vector<MeshColor>              colors;
        std::vector<MeshScalar>             scalars;
        GLuint vao, vbo, ebo;
        glm::mat4 modelTransformation;
        bool instancesInitialized = false;

        void setupMesh();
        void setUniforms(Shader& shader, glm::mat4 scene_model_matrix);

        void resetUniforms(Shader& shader);
};

#endif
#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <iostream>
#include <vector>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <plum/scenenode.hpp>
#include <plum/shader.hpp>
#include <plum/texture.hpp>

std::vector<float> cubeData = std::vector<float>({
    // positions          // normals           // uv        // tangent
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f
});

class Shape : public Drawable {
    public:

        enum Shape_Surface {
            SHAPE_SURFACE_COLOR,
            SHAPE_SURFACE_TEXTURE,
            SHAPE_SURFACE_HYBRID
        };

        std::map<Tex::Tex_Type, std::shared_ptr<Tex>> Textures;
        Shape_Surface SurfaceType = SHAPE_SURFACE_COLOR;
        static Shader* DefaultShaderColorOnly;
        static Shader* DefaultShaderTextureOnly;
        static Shader* DefaultShaderHybrid;

        ~Shape();

        // Methods
        virtual void Draw(Shader& shader, glm::mat4 model_matrix = glm::identity<glm::mat4>()) override = 0;
        
        // Modifiers
        void SetColor(glm::vec3 col);
        void SetMetallic(float met);
        void SetRoughness(float rough);
        void SetTexture(Tex::Tex_Type type, std::shared_ptr<Tex> texture);
        void SetSurfaceType(Shape_Surface type);
        void SetPipelineBypass(const bool bypass);
        
        // Accessors
        glm::vec3 GetColor();
        glm::vec3* GetColorPtr();
        float GetMetallic();
        float* GetMetallicPtr();
        float GetRoughness();
        float* GetRoughnessPtr();
        Shape_Surface GetSurfaceType();
        bool GetPipelineBypass();

    protected:
        bool bypass_pipeline = false;

        GLuint vao, vbo, ebo;

        std::vector<float>* data;            // Generally for raw, interleaved data
        std::vector<glm::vec3> positions;    // Generally for compile- or run-time data
        std::vector<glm::vec3> normals;      // Generally for compile- or run-time data
        std::vector<glm::vec2> uv;           // Generally for compile- or run-time data
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;

        glm::vec3 albedo = glm::vec3(0.5);   // Default gray color
        float metallic = 0.0;
        float roughness = 0.5;
        
        Shape(std::string name, Shader* default_shader);
        virtual void setup() = 0;
        void setUniforms(Shader& shader, glm::mat4 model_matrix);
        void resetUniforms(Shader& shader);
        
};

Shader* Shape::DefaultShaderColorOnly = nullptr;
Shader* Shape::DefaultShaderTextureOnly = nullptr;
Shader* Shape::DefaultShaderHybrid = nullptr;

class Cube : public Shape {
    public:
        Cube(std::string name, Shader* default_shader = nullptr);
        void Draw(Shader& shader, glm::mat4 model_matrix = glm::identity<glm::mat4>()) override;
        void DrawInstanced();


    private:
        void setup() override;
};

class Sphere : public Shape {
    public:
        int StackCount;
        int SectorCount;

        Sphere(std::string name, Shader* default_shader = nullptr, const int stack_count = 30, const int sector_count = 30);
        void Draw(Shader& shader, glm::mat4 model_matrix = glm::identity<glm::mat4>()) override;
        void DrawInstanced(Shader& s, GLsizei count);

    private:
        std::vector<unsigned int> indices;
        std::vector<unsigned int> lineIndices;

        void generateVertexData(const int stack_count, const int sector_count);

        void setup() override;
};

class Rectangle : public Shape {
    public:
        Rectangle(std::string name, Shader* default_shader = nullptr, const unsigned int x_sections = 1, const unsigned int z_sections = 1);
        void Draw(Shader& shader, glm::mat4 model_matrix = glm::identity<glm::mat4>()) override;
    private:
        unsigned int xSections, zSections;

        void setup() override;
};

#endif

#include <plum/shape.hpp>

#include <iostream>
#include <vector>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <plum/scenenode.hpp>
#include <plum/shader.hpp>
#include <plum/texture.hpp>

#include <plum/component/core.hpp>
#include <plum/component/vertex.hpp>

// ======= Shape =======

Shader* Shape::DefaultShaderColorOnly = nullptr;
Shader* Shape::DefaultShaderTextureOnly = nullptr;
Shader* Shape::DefaultShaderHybrid = nullptr;

Shape::~Shape() {
    // glDeleteVertexArrays(1, &vao);
    // glDeleteBuffers(1, &vbo);
    // glDeleteBuffers(1, &ebo);
}

// Modifiers
void Shape::SetColor(glm::vec3 col) {
    albedo = col;
    SurfaceType = SHAPE_SURFACE_COLOR;
}
void Shape::SetMetallic(float met) {
    metallic = met;
}
void Shape::SetRoughness(float rough) {
    roughness = rough;
}
void Shape::SetTexture(Tex::Tex_Type type, std::shared_ptr<Tex> texture) {
    Textures[type] = texture;
    SurfaceType = SHAPE_SURFACE_TEXTURE;
}
void Shape::SetSurfaceType(Shape_Surface type) { 
    SurfaceType = type;
    switch (type) {
        case SHAPE_SURFACE_COLOR:
            DefaultShader = DefaultShaderColorOnly;
            break;
        case SHAPE_SURFACE_TEXTURE:
            DefaultShader = DefaultShaderTextureOnly;
            break;
        case SHAPE_SURFACE_HYBRID:
            DefaultShader = DefaultShaderHybrid;
            break;
    }
}
void Shape::SetPipelineBypass(const bool bypass) { bypass_pipeline = bypass; }

// Accessors
glm::vec3 Shape::GetColor() { return albedo; }
glm::vec3* Shape::GetColorPtr() { return &albedo; }
float Shape::GetMetallic() { return metallic; }
float* Shape::GetMetallicPtr() { return &metallic; }
float Shape::GetRoughness() { return roughness; }
float* Shape::GetRoughnessPtr() { return &roughness; }
Shape::Shape_Surface Shape::GetSurfaceType() { return SurfaceType; }
bool Shape::GetPipelineBypass() { return bypass_pipeline; }


Shape::Shape(std::string name, Shader* default_shader) : Drawable(name, default_shader) {}
void Shape::setUniforms(Shader& shader, glm::mat4 model_matrix) {
    // Model transform
    shader.setMat4("model", model_matrix);
    // PBR metallic-roughness
    shader.setVec3("material.albedo", albedo);
    shader.setFloat("material.metallic", metallic);
    shader.setFloat("material.roughness", roughness);
    // Textures
    if (SurfaceType == SHAPE_SURFACE_TEXTURE || SurfaceType == SHAPE_SURFACE_HYBRID) {
        int texcount = 0;
        for (auto& texture : Textures) {
            glActiveTexture(GL_TEXTURE0 + texcount);
            switch (texture.first) {
                case Tex::Tex_Type::TEX_AMBIENT:
                    shader.setInt("material.texture_ambient", texcount++);
                    shader.setInt("material.texture_ambient_count", 1);
                    break;
                case Tex::Tex_Type::TEX_DIFFUSE:
                    shader.setInt("material.texture_diffuse", texcount++);
                    shader.setInt("material.texture_diffuse_count", 1);
                    break;
                case Tex::Tex_Type::TEX_SPECULAR:
                    shader.setInt("material.texture_specular", texcount++);
                    shader.setInt("material.texture_specular_count", 1);
                    break;
                case Tex::Tex_Type::TEX_HEIGHT:
                    shader.setInt("material.texture_height", texcount++);
                    shader.setInt("material.texture_height_count", 1);
                    break;
                case Tex::Tex_Type::TEX_NORMAL:
                    shader.setInt("material.texture_normal", texcount++);
                    shader.setInt("material.texture_normal_count", 1);
                    break;
                case Tex::Tex_Type::TEX_ROUGHNESS:
                    shader.setInt("material.texture_roughness", texcount++);
                    shader.setInt("material.texture_roughness_count", 1);
                    break;
                case Tex::Tex_Type::TEX_METALNESS:
                    shader.setInt("material.texture_metalness", texcount++);
                    shader.setInt("material.texture_metalness_count", 1);
                    break;
                case Tex::Tex_Type::TEX_OCCLUSION:
                    shader.setInt("material.texture_occlusion", texcount++);
                    shader.setInt("material.texture_occlusion_count", 1);
                    break;
                case Tex::Tex_Type::TEX_UNKNOWN:
                    break;
            }
            glBindTexture(GL_TEXTURE_2D, texture.second->ID);
        }
    }
    glActiveTexture(GL_TEXTURE0);
}
void Shape::resetUniforms(Shader& shader) {
    if (SurfaceType == SHAPE_SURFACE_TEXTURE || SurfaceType == SHAPE_SURFACE_HYBRID) {
        for (auto& texture : Textures) {
            switch (texture.first) {
                case Tex::Tex_Type::TEX_AMBIENT:
                    shader.setInt("material.texture_ambient_count", 0);
                    break;
                case Tex::Tex_Type::TEX_DIFFUSE:
                    shader.setInt("material.texture_diffuse_count", 0);
                    break;
                case Tex::Tex_Type::TEX_SPECULAR:
                    shader.setInt("material.texture_specular_count", 0);
                    break;
                case Tex::Tex_Type::TEX_HEIGHT:
                    shader.setInt("material.texture_height_count", 0);
                    break;
                case Tex::Tex_Type::TEX_NORMAL:
                    shader.setInt("material.texture_normal_count", 0);
                    break;
                case Tex::Tex_Type::TEX_ROUGHNESS:
                    shader.setInt("material.texture_roughness_count", 0);
                    break;
                case Tex::Tex_Type::TEX_METALNESS:
                    shader.setInt("material.texture_metalness_count", 0);
                    break;
                case Tex::Tex_Type::TEX_OCCLUSION:
                    shader.setInt("material.texture_occlusion_count", 0);
                    break;
                case Tex::Tex_Type::TEX_UNKNOWN:
                    break;
            }
        }
    }
}


// ======= Cube =======

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

Cube::Cube(std::string name, Shader* default_shader) : Shape(name, default_shader) {
    setup();
}
void Cube::Draw(Shader& shader, glm::mat4 model_matrix) {
    glUseProgram(shader.programID);
    setUniforms(shader, model_matrix);
    vao->Draw();
    resetUniforms(shader);
}
void Cube::DrawInstanced() {}

void Cube::setup() {
    Component::VertexArray vertices(cubeData, Component::VertexAttrFlags::All ^ Component::VertexAttrFlags::Bitangent);
    auto vbo = std::make_shared<Component::Vbo>(vertices);
    vao = std::make_shared<Component::Vao>(vbo);
}

// ======= Sphere =======

Sphere::Sphere(std::string name, Shader* default_shader, const int stack_count, const int sector_count) : Shape(name, default_shader) {
    StackCount = stack_count;
    SectorCount = sector_count;
    generateVertexData(StackCount, SectorCount);
    setup();
}
void Sphere::Draw(Shader& shader, glm::mat4 model_matrix) {
    glUseProgram(shader.programID);
    setUniforms(shader, model_matrix);
    vao->Draw();
    resetUniforms(shader);
}
void Sphere::DrawInstanced(Shader& s, GLsizei count) {}

void Sphere::generateVertexData(const int stack_count, const int sector_count) {
    // Generate vertices, normals, uv
    positions.clear();
    normals.clear();
    uv.clear();
    for (int i = 0; i <= stack_count; i++) {
        float stack_angle = glm::pi<float>() * (-0.5 + float(i) / stack_count);    // bottom up
        float y = glm::sin(stack_angle);
        std::vector<glm::vec3> stack_positions;
        for (int j = 0; j <= sector_count; j++) {
            // Positions, normals, texture coords
            float sector_angle = glm::two_pi<float>() * (float(j) / sector_count); // clockwise when looking in -y direction
            float x = glm::cos(stack_angle) * glm::cos(sector_angle);
            float z = glm::cos(stack_angle) * glm::sin(sector_angle);
            positions.insert(positions.end(), {x,y,z});
            normals.insert(normals.end(), {x,y,z});
            uv.insert(uv.end(), {float(j)/sector_count, float(i)/stack_count});
            stack_positions.push_back(glm::vec3(x,y,z));
            // Tangents, bitangents for top and bottom edge cases
            if (i == 0 || i == stack_count) {
                float fake_stack_angle = stack_angle + 0.01;
                float fake_x = glm::cos(fake_stack_angle) * glm::cos(fake_stack_angle);
                float fake_z = glm::cos(fake_stack_angle) * glm::sin(fake_stack_angle);
                glm::vec3 bitangent = glm::normalize( glm::vec3(fake_x,0,fake_z) );
                bitangents.push_back(bitangent.x);
                bitangents.push_back(bitangent.y);
                bitangents.push_back(bitangent.z);
                glm::vec3 tangent;
                if (i == 0) {
                    tangent = glm::normalize( glm::cross(glm::vec3(0,1,0), bitangent) );
                    tangents.push_back(tangent.x);
                    tangents.push_back(tangent.y);
                    tangents.push_back(tangent.z);
                } else if (i == stack_count) {
                    tangent = glm::normalize( glm::cross(glm::vec3(0,-1,0), bitangent) );
                } 
                tangents.push_back(tangent.x);
                tangents.push_back(tangent.y);
                tangents.push_back(tangent.z);
            }
        }
        // Work backwards to compute non-edgecase tangents and bitangents
        glm::vec3 lastpos = stack_positions.front();
        for (auto currpos = stack_positions.rbegin(); currpos != stack_positions.rend(); currpos++) {
            if (i == 0 || i == stack_count) {
                continue;
            } else {
                glm::vec3 bitangent = glm::normalize( glm::cross(*currpos, lastpos) );
                bitangents.push_back(bitangent.x);
                bitangents.push_back(bitangent.y);
                bitangents.push_back(bitangent.z);
                glm::vec3 tangent = glm::normalize( glm::cross(bitangent, *currpos) );
                tangents.push_back(tangent.x);
                tangents.push_back(tangent.y);
                tangents.push_back(tangent.z);
                lastpos = *currpos;
            }
        }
    }
    // Generate indices
    indices.clear();
    lineIndices.clear();
    for (int i = 0; i < stack_count; i++) {
        int ind1 = i * (sector_count + 1);        // first index in current stack
        int ind2 = (i + 1) * (sector_count + 1);  // first index in stack immediately above
        for (int j = 0; j < sector_count; j++) {
            // Triangle 1 (CCW)
            if (i != sector_count - 1) {
                indices.push_back(ind1);
                indices.push_back(ind2);
                indices.push_back(ind2 + 1);
            }
            // Triangle 2 (CCW)
            if (i != 0) {
                indices.push_back(ind1);
                indices.push_back(ind2 + 1);
                indices.push_back(ind1 + 1);
            }
            // Vertical line
            lineIndices.push_back(ind1);
            lineIndices.push_back(ind2);
            // Horizontal line
            if (i != 0) {
                lineIndices.push_back(ind1);
                lineIndices.push_back(ind1 + 1);
            }
            ind1++;
            ind2++;
        }
    }            
}

void Sphere::setup() {
    Component::VertexArray::UncollatedVertices vertices = {positions, normals, uv, tangents, bitangents};
    auto vbo = std::make_shared<Component::Vbo>(vertices);
    auto ebo = std::make_shared<Component::Ebo>(indices);
    vao = std::make_shared<Component::Vao>(vbo, ebo);
}


// ======= Rectangle =======

Rectangle::Rectangle(std::string name, Shader* default_shader, const unsigned int x_sections, const unsigned int z_sections) 
: Shape(name, default_shader) {
    xSections = x_sections;
    zSections = z_sections;
    setup();
}
void Rectangle::Draw(Shader& shader, glm::mat4 model_matrix) {
    glUseProgram(shader.programID);
    glDisable(GL_CULL_FACE);
    setUniforms(shader, model_matrix);
    vao->Draw();
    resetUniforms(shader);
    glEnable(GL_CULL_FACE);
}

void Rectangle::setup() {
    std::vector<float> positions = {    // Remember CCW (x-z vs x-y)
        -1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 1.0f
    };
    std::vector<float> normals = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    std::vector<float> uvs = {
        0.0f,              0.0f,
        0.0f,              1.0f * zSections,
        1.0f * xSections, 0.0f,
        1.0f * xSections, 1.0f * zSections,
        1.0f * xSections, 0.0f,
        0.0f,              1.0f * zSections
    };
    std::vector<float> tangents = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    Component::VertexArray::UncollatedVertices vertices = {positions, normals, uvs, tangents};
    auto vbo = std::make_shared<Component::Vbo>(vertices);
    vao = std::make_shared<Component::Vao>(vbo);
}

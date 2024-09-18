#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <vector>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <plum/engine.hpp>
#include <plum/light.hpp>
#include <plum/shader.hpp>
#include <plum/texture.hpp>
#include <plum/vertex.hpp>

using namespace std;

struct MeshColor {
    glm::vec3 Value;
    string type; // diffuse, specular, etc.
};
struct MeshScalar {
    float Value;
    string type;
};

class Mesh {
    public:        
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<shared_ptr<Tex>> textures, vector<MeshColor> colors, vector<MeshScalar> scalars, glm::mat4 model_transformation) 
        : vertices(vertices), indices(indices), textures(textures), colors(colors), scalars(scalars), modelTransformation(model_transformation)
        {
            setupMesh();
        };
        ~Mesh() {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }

        void Draw(Shader& shader, glm::mat4 scene_model_matrix) {

            setUniforms(shader, scene_model_matrix);
            
            // Draw
            glBindVertexArray(vao);
            if (indices.size() > 0) {
                glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, vertices.size());
            }
            
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);

            resetUniforms(shader);
        };

        void DrawInstanced(Shader& shader, GLsizei instancecount) {

            setUniforms(shader, glm::mat4(1));
            
            // Draw
            glBindVertexArray(vao);
            
            if (!instancesInitialized) {
                glEnableVertexAttribArray(3);   
                glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)0);
                glEnableVertexAttribArray(4);   
                glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(1 * sizeof(glm::vec4)));
                glEnableVertexAttribArray(5);   
                glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
                glEnableVertexAttribArray(6);   
                glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));
                
                glVertexAttribDivisor(3, 1);
                glVertexAttribDivisor(4, 1);
                glVertexAttribDivisor(5, 1);
                glVertexAttribDivisor(6, 1);

                instancesInitialized = true;
            }

            if (indices.size() > 0) {
                glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancecount);
            } else {
                glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), instancecount);
            }
            
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
        };
    

    private:
        vector<Vertex>              vertices;
        vector<unsigned int>        indices;
        vector<shared_ptr<Tex>>     textures;
        vector<MeshColor>           colors;
        vector<MeshScalar>          scalars;
        GLuint vao, vbo, ebo;
        glm::mat4 modelTransformation;
        bool instancesInitialized = false;

        void setupMesh() {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            glBindVertexArray(vao);
            // vbo data
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
            // ebo data
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

            // Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
            // Normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
            // MeshTexture coordinates
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));
            // Tangents and bitangents
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

            glBindVertexArray(0);
                // // Error check
            // GLint size;
            // glad_glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            // if (size != sizeof(vertices)) {
            //     glad_glDeleteBuffers(1, &vbo);
            //     return -1;
            // }
            // glad_glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
            // if (size != sizeof(indices)) {
            //     glad_glDeleteBuffers(1, &ebo);
            //     return -1;
            // }
        };

        void setUniforms(Shader& shader, glm::mat4 scene_model_matrix) {            
            // Model transform
            shader.setMat4("model", scene_model_matrix * modelTransformation);
            // Textures
            unsigned int diffuseNum = 0;
            unsigned int specularNum = 0;
            unsigned int ambientNum = 0;
            unsigned int heightNum = 0;
            unsigned int normalNum = 0;
            unsigned int roughnessNum = 0;
            unsigned int metalnessNum = 0;
            unsigned int occlusionNum = 0;
            unsigned int unknownNum = 0;

            for (int i = 0; i < textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                string number;
                Tex_Type type = textures[i]->Type;
                switch (type) {
                    case TEX_AMBIENT:
                        number = to_string(ambientNum++);
                        shader.setInt("material.texture_ambient_count", ambientNum);
                        break;
                    case TEX_DIFFUSE:
                        number = to_string(diffuseNum++);
                        shader.setInt("material.texture_diffuse_count", diffuseNum);
                        break;
                    case TEX_SPECULAR:
                        number = to_string(specularNum++);
                        shader.setInt("material.texture_specular_count", specularNum);
                        break;
                    case TEX_HEIGHT:
                        number = to_string(heightNum++);
                        shader.setInt("material.texture_height_count", heightNum);
                        break;
                    case TEX_NORMAL:
                        number = to_string(normalNum++);
                        shader.setInt("material.texture_normal_count", normalNum);
                        break;
                    case TEX_METALNESS:
                        number = to_string(metalnessNum++);
                        shader.setInt("material.texture_metalness_count", metalnessNum);
                        break;
                    case TEX_ROUGHNESS:
                        number = to_string(roughnessNum++);
                        shader.setInt("material.texture_roughness_count", roughnessNum);
                        break;
                    case TEX_OCCLUSION:
                        number = to_string(occlusionNum++);
                        shader.setInt("material.texture_occlusion_count", occlusionNum);
                        break;
                    case TEX_UNKNOWN:
                        number = to_string(unknownNum++);
                        shader.setInt("material.texture_unknown_count", unknownNum);
                        break;
                }
                // shader.setInt(("material." + type + "[" + number + "]").c_str(), i);
                string stringtype = TexTypeToString(type);
                shader.setInt(("material.texture_" + stringtype).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, textures[i]->ID);
            }
            
            // Colors
            for (auto& color : colors) {
                shader.setVec3(("material." + color.type).c_str(), color.Value);
            }
            // Other properties
            for (auto scalar : scalars) {
                shader.setFloat(("material." + scalar.type).c_str(), scalar.Value);
            }

            glActiveTexture(GL_TEXTURE0);
        }

        void resetUniforms(Shader& shader) {
            for (auto& texture : textures) {
                switch (texture->Type) {
                    case TEX_AMBIENT:
                        shader.setInt("material.texture_ambient_count", 0);
                        break;
                    case TEX_DIFFUSE:
                        shader.setInt("material.texture_diffuse_count", 0);
                        break;
                    case TEX_SPECULAR:
                        shader.setInt("material.texture_specular_count", 0);
                        break;
                    case TEX_HEIGHT:
                        shader.setInt("material.texture_height_count", 0);
                        break;
                    case TEX_NORMAL:
                        shader.setInt("material.texture_normal_count", 0);
                        break;
                    case TEX_ROUGHNESS:
                        shader.setInt("material.texture_roughness_count", 0);
                        break;
                    case TEX_METALNESS:
                        shader.setInt("material.texture_metalness_count", 0);
                        break;
                    case TEX_OCCLUSION:
                        shader.setInt("material.texture_occlusion_count", 0);
                        break;
                    case TEX_UNKNOWN:
                        shader.setInt("material.texture_unknown_count", 0);
                        break;
                }
            }
        }
};

#endif
#pragma once

#include <plum/component/component.hpp>
#include <plum/component/mesh.hpp>
#include <plum/material/material.hpp>
#include <plum/material/texture.hpp>
#include <plum/util/transform.hpp>

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Component {

    struct MaterialInfo {
        bool isDefault = true;
        glm::vec3 ambient = glm::vec3(1,1,0), diffuse = glm::vec3(1,1,0), specular = glm::vec3(1,1,0);
        float metalness = -1, roughness = -1, glossiness = -1;
        std::vector<std::shared_ptr<Material::Texture>> textures;
    };

    class Model;

    // or do we just create a new node with multiple meshes
    class ModelNode : public ComponentBase {
        private:
            static const std::map<aiTextureType, Material::TextureType> textureTypeMap;

        public:
            ModelNode(Model& model, aiNode* ainode, const aiScene* aiscene);
            ~ModelNode();

            void Draw(const glm::mat4& model_matrix);
            void Draw(Material::MaterialBase& material, const glm::mat4& parent_transform);
            void Draw(Material::Module& module, const glm::mat4& parent_transform);

            Transform transform;

            std::vector<std::shared_ptr<ModelNode>> children;

        private:
            Model& model;
            std::vector<std::shared_ptr<Mesh>> meshes;

            std::shared_ptr<Mesh> processMesh(aiMesh* aimesh, const aiScene* scene);
            MaterialInfo processMaterial(aiMaterial* aimaterial);
            std::vector<std::shared_ptr<Material::Texture>> loadMaterialTextures(aiMaterial* aimaterial, aiTextureType aitextype);
    };
        
    class Model : public ComponentBase {
        public:
            Model(std::string path, float scale = 1.0f, bool flipUvs = false, GLuint wrap = GL_REPEAT);
            ~Model();
            
            void Draw(const glm::mat4& model_matrix);
            void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix);
            void Draw(Material::Module& module, const glm::mat4& model_matrix);
            void Animate(float speed = 1.f, bool interpolate = true);
            
            // bones and animation
            std::string name;
            std::string path;
            std::string directory;
            
            std::vector<std::shared_ptr<Material::Texture>> textures;
            std::shared_ptr<ModelNode> root;
    
        private:    
            GLuint wrap;
            std::map<std::string, bool> necessityMap;

            void importFile(const std::string& filename, float scale, bool flipUvs);
            void printSceneInfo(const std::string& path, const aiScene *scene, const std::string& outpath = "");
    };

    class Animation {
        float duration;
        float ticksPerSecond;
        std::vector<glm::vec3> positionKeys;
        std::vector<glm::quat> quaternionKeys;
        std::vector<glm::vec3> scaleKeys;

    };

}
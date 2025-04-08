#pragma once

#include <plum/component/component.hpp>
#include <plum/component/mesh.hpp>
#include <plum/context/asset.hpp>
#include <plum/material/material.hpp>
#include <plum/material/texture.hpp>
#include <plum/util/transform.hpp>
#include <plum/util/color.hpp>

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Component {

    struct MaterialInfo {
        // bool isDefault = true;
        glm::vec3 diffuse = Color::yellow;
        glm::vec3 specular = Color::yellow;
        glm::vec3 ambient = Color::yellow;
        float metalness = -1;
        float roughness = -1;
        float glossiness = -1;
        std::vector<std::shared_ptr<Material::Texture>> textures;
    };

    class ModelNode;
        
    class Model : public ComponentBase, public Asset::Asset {
        public:
            Model(Path path, float scale = 1.0f, bool flipUvs = false, GLuint wrap = GL_REPEAT);
            ~Model();
            
            void Draw(const glm::mat4& model_matrix) override;
            void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix) override;
            void Draw(Renderer::Module& module, const glm::mat4& model_matrix) override;
            void Animate(float speed = 1.f, bool interpolate = true);
            
            // bones and animation
            
            std::vector<std::shared_ptr<Material::Texture>> textures;
            std::shared_ptr<ModelNode> root;

            // Asset::Asset
            void SyncWithDevice() override;
    
        private:    
            float scale;
            bool flipUvs;
            GLuint wrap;
            // std::map<std::string, bool> necessityMap;

            void importFile(const fs::path& path, float scale, bool flipUvs);
            void printSceneInfo(const std::string& path, const aiScene *scene, const std::string& outpath = "");
    };

    // or do we just create a new node with multiple meshes
    class ModelNode : public ComponentBase {
        private:
            static const std::map<aiTextureType, Material::TextureType> textureTypeMap;

        public:
            ModelNode(Model& model, aiNode* ainode, const aiScene* aiscene);
            ~ModelNode() = default;

            void Draw(const glm::mat4& model_matrix);
            void Draw(Material::MaterialBase& material, const glm::mat4& parent_transform);
            void Draw(Renderer::Module& module, const glm::mat4& parent_transform);

            Transform transform;

            std::vector<std::shared_ptr<ModelNode>> children;

        private:
            Model& model;
            std::vector<std::shared_ptr<Mesh>> meshes;

            std::shared_ptr<Mesh> processMesh(aiMesh* aimesh, const aiScene* scene);
            MaterialInfo processMaterial(aiMaterial* aimaterial);
            std::vector<std::shared_ptr<Material::Texture>> loadMaterialTextures(aiMaterial* aimaterial, aiTextureType aitextype);
    };

    class Animation {
        float duration;
        float ticksPerSecond;
        std::vector<glm::vec3> positionKeys;
        std::vector<glm::quat> quaternionKeys;
        std::vector<glm::vec3> scaleKeys;

    };

}
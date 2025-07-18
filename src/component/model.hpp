#pragma once

#include "component/component.hpp"
#include "component/mesh.hpp"
#include "asset/model.hpp"
#include "interface/widget.hpp"
#include "material/material.hpp"
#include "material/texture.hpp"
#include "util/transform.hpp"
#include "util/color.hpp"

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
        float metalness = -1.f;
        float roughness = -1.f;
        float glossiness = -1.f;
        std::vector<std::shared_ptr<Material::Texture>> textures;
    };

    class ModelNode;
        
    class Model : public ComponentBase, public AssetUser {
        public:
            Model(std::shared_ptr<ModelAsset> model);
            ~Model();
            
            std::shared_ptr<ModelAsset> model;
            std::shared_ptr<ModelNode> root;
            std::vector<std::shared_ptr<Material::Texture>> textures;
            
            void Draw(const glm::mat4& model_matrix) override;
            void Draw(Material::MaterialBase& material, const glm::mat4& model_matrix) override;
            void Draw(Renderer::RenderModule& module, const glm::mat4& model_matrix) override;

            void AssetResyncCallback() override;
            
            virtual std::shared_ptr<ComponentBase> Duplicate() override { return nullptr; } // future
            
            // GUI widget
            void DisplayWidget() override;

        private:
            void printSceneInfo(const std::string& path, const aiScene *scene, const std::string& outpath = "");
    };

    // or do we just create a new node with multiple meshes?
    class ModelNode : public ComponentBase {
        public:
            ModelNode(Model& model, aiNode* ainode, const aiScene* aiscene);
            ~ModelNode() = default;
            
            Transform transform;
            std::vector<std::shared_ptr<ModelNode>> children;

            void Draw(const glm::mat4& model_matrix) override;
            void Draw(Material::MaterialBase& material, const glm::mat4& parent_transform) override;
            void Draw(Renderer::RenderModule& module, const glm::mat4& parent_transform) override;

            std::shared_ptr<ComponentBase> Duplicate() override {return nullptr;}
        
        private:
            static const std::map<aiTextureType, Material::TextureType> textureTypeMap;
            Model& head;
            std::vector<std::shared_ptr<Mesh>> meshes;

            std::shared_ptr<Mesh> processMesh(aiMesh* aimesh, const aiScene* scene);
            MaterialInfo processMaterial(aiMaterial* aimaterial);
            std::vector<std::shared_ptr<Material::Texture>> loadMaterialTextures(aiMaterial* aimaterial, aiTextureType aitextype);
    };

}
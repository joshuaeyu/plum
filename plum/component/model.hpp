#pragma once

#include <plum/component/component.hpp>
#include <plum/component/mesh.hpp>
#include <plum/asset/model.hpp>
#include <plum/interface/widget.hpp>
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

            void DisplayWidget() override {};    
            void AssetResyncCallback() override;

            std::shared_ptr<ComponentBase> Duplicate() override {return nullptr;}
        
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
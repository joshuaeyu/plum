#include <plum/component/model.hpp>

#include <plum/asset/manager.hpp>
#include <plum/component/mesh.hpp>
#include <plum/core/globject.hpp>
#include <plum/material/material.hpp>
#include <plum/material/texture.hpp>
#include <plum/util/transform.hpp>

#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>

#include <iostream>
#include <fstream>
#include <set>
#include <memory>
#include <string>
#include <filesystem>

namespace Component {

    const std::map<aiTextureType, Material::TextureType> ModelNode::textureTypeMap = {
        {aiTextureType_DIFFUSE,             Material::TextureType::Diffuse},
        {aiTextureType_SPECULAR,            Material::TextureType::Specular},
        {aiTextureType_AMBIENT,             Material::TextureType::Ambient},
        {aiTextureType_EMISSIVE,            Material::TextureType::Emissive},
        {aiTextureType_HEIGHT,              Material::TextureType::Normal},
        {aiTextureType_NORMALS,             Material::TextureType::Normal},
        {aiTextureType_SHININESS,           Material::TextureType::Normal},
        {aiTextureType_OPACITY,             Material::TextureType::Normal},
        {aiTextureType_DISPLACEMENT,        Material::TextureType::Height},
        {aiTextureType_LIGHTMAP,            Material::TextureType::Occlusion},
        {aiTextureType_REFLECTION,          Material::TextureType::Occlusion},
        
        // PBR
        {aiTextureType_BASE_COLOR,          Material::TextureType::Diffuse},
        {aiTextureType_NORMAL_CAMERA,       Material::TextureType::Normal},
        {aiTextureType_EMISSION_COLOR,      Material::TextureType::Emissive},
        {aiTextureType_METALNESS,           Material::TextureType::Metalness},
        {aiTextureType_DIFFUSE_ROUGHNESS,   Material::TextureType::Roughness},
        {aiTextureType_AMBIENT_OCCLUSION,   Material::TextureType::Occlusion},
        {aiTextureType_UNKNOWN,             Material::TextureType::Unknown}
    };

    Model::Model(std::shared_ptr<ModelAsset> model)
        : ComponentBase(ComponentType::Model),
        model(model)
    {
        name = "Model";
        // name = files[0].Name();
        root = std::make_shared<ModelNode>(*this, model->Scene()->mRootNode, model->Scene());
    }
    Model::~Model() {}
    
    void Model::Draw(const glm::mat4& model_matrix) {
        root->Draw(model_matrix);
    }
    void Model::Draw(Material::MaterialBase& material, const glm::mat4& model_matrix) {
        root->Draw(material, model_matrix);
    }
    void Model::Draw(Renderer::Module& module, const glm::mat4& model_matrix) {
        root->Draw(module, model_matrix);
    }

    void Model::AssetResyncCallback() {
        root = std::make_shared<ModelNode>(*this, model->Scene()->mRootNode, model->Scene());
    }

    void Model::printSceneInfo(const std::string& path, const aiScene *scene, const std::string& outpath) {            
        // Check if model's scene info was already logged
        if (outpath != "") {
            std::ifstream f(outpath);
            std::string buffer;
            while (getline(f, buffer)) {
                if (buffer.find(path) != std::string::npos) {
                    f.close();
                    std::clog << "    Scene info already logged in file " << outpath << std::endl;
                    return;
                }
            }
            f.close();
        }
        
        std::stringstream ss;
        
        // Print model path
        ss << path << std::endl;
    
        // Print count of each scene component
        ss << "    mNumAnimations:  " << scene->mNumAnimations << std::endl;
        ss << "    mNumCameras:     " << scene->mNumCameras << std::endl;
        ss << "    mNumLights:      " << scene->mNumLights << std::endl;
        ss << "    mNumMaterials:   " << scene->mNumMaterials << std::endl;
        for (int i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial *mat = scene->mMaterials[i];
            std::clog << "      " << mat->GetName().C_Str() << std::endl;
        }
        ss << "    mNumMeshes:      " << scene->mNumMeshes << std::endl;
        ss << "    mNumSkeletons:   " << scene->mNumSkeletons << std::endl;
        ss << "    mNumTextures:    " << scene->mNumTextures << std::endl;
    
        // Print all material properties
        std::set<std::string> materialProperties;
    
        for (int i = 0; i < scene->mNumMaterials; i++){
            // aiMaterial *materiali = scene->mMaterials[i]; // first material is Gourad (2), rest are Phong (3)
            // float value;
            // materiali->Get(AI_MATKEY_OPACITY, value);
            // std::clog << "material " << i << " opacity: " << value << std::endl;
            for (int j = 0; j < scene->mMaterials[i]->mNumProperties; j++) {
                materialProperties.insert(scene->mMaterials[i]->mProperties[j]->mKey.C_Str());
            }
        }
        int i = 0;
        for (auto it = materialProperties.begin(); it != materialProperties.end(); it++) {
            ss << "    Material property " << i++ << ":\t" << *it << std::endl;
        }
    
        // Write to file or std::clog
        if (outpath != "") {
            std::ofstream f(outpath, std::ofstream::app);
            if (f.bad()) {
                std::cerr << "Error opening outfile " << outpath << std::endl;
            } else {
                f << ss.rdbuf() << std::endl;
                f.close();
            }
        } else {
            std::clog << ss.rdbuf() << std::endl;
        }
    }
    
    ModelNode::ModelNode(Model& head, aiNode* ainode, const aiScene* aiscene) 
        : ComponentBase(ComponentType::Model),
        head(head)
    {
        // Node model transform
        glm::mat4 matrix;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                matrix[i][j] = ainode->mTransformation[j][i];
            }
        }
        transform = Transform(matrix);
        // Meshes
        for (int i = 0; i < ainode->mNumMeshes; i++) {
            aiMesh *aimesh = aiscene->mMeshes[ainode->mMeshes[i]];
            meshes.push_back(processMesh(aimesh, aiscene));
        }
        // Children
        for (int i = 0; i < ainode->mNumChildren; i++) {
            children.push_back(std::make_shared<ModelNode>(head, ainode->mChildren[i], aiscene));
        }
    }

    void ModelNode::Draw(const glm::mat4& parent_transform) {
        const glm::mat4 model_matrix = parent_transform * transform.Matrix();
        for (auto& mesh : meshes) {
            mesh->Draw(model_matrix);
        }
        for (auto& child : children) {
            child->Draw(model_matrix);
        }
    }
    void ModelNode::Draw(Material::MaterialBase& material, const glm::mat4& parent_transform) {
        const glm::mat4 model_matrix = parent_transform * transform.Matrix();
        for (auto& mesh : meshes) {
            mesh->Draw(material, model_matrix);
        }
        for (auto& child : children) {
            child->Draw(material, model_matrix);
        }
    }
    void ModelNode::Draw(Renderer::Module& module, const glm::mat4& parent_transform) {
        const glm::mat4 model_matrix = parent_transform * transform.Matrix();
        for (auto& mesh : meshes) {
            mesh->Draw(module, model_matrix);
        }
        for (auto& child : children) {
            child->Draw(module, model_matrix);
        }
    }

    std::shared_ptr<Mesh> ModelNode::processMesh(aiMesh* aimesh, const aiScene* scene) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        
        using Core::Vertex::AttrFlags;
        AttrFlags vertexAttrFlags = Core::Vertex::AttrFlags::None;
        if (aimesh->HasPositions())             vertexAttrFlags |= AttrFlags::Position3;
        if (aimesh->HasNormals())               vertexAttrFlags |= AttrFlags::Normal;
        if (aimesh->HasTextureCoords(0))        vertexAttrFlags |= AttrFlags::Uv;  
        if (aimesh->HasTangentsAndBitangents()) vertexAttrFlags |= AttrFlags::Tangent | AttrFlags::Bitangent;
        
        // VERTICES
        for (int i = 0; i < aimesh->mNumVertices; i++) {
            if (aimesh->HasPositions()) {
                vertices.push_back(aimesh->mVertices[i].x);
                vertices.push_back(aimesh->mVertices[i].y);
                vertices.push_back(aimesh->mVertices[i].z);
            }
            if (aimesh->HasNormals()) {
                vertices.push_back(aimesh->mNormals[i].x);
                vertices.push_back(aimesh->mNormals[i].y);
                vertices.push_back(aimesh->mNormals[i].z);
            }
            if (aimesh->HasTextureCoords(0)) {
                vertices.push_back(aimesh->mTextureCoords[0][i].x);
                vertices.push_back(aimesh->mTextureCoords[0][i].y);
            }
            if (aimesh->HasTangentsAndBitangents()) {
                vertices.push_back(aimesh->mTangents[i].x);
                vertices.push_back(aimesh->mTangents[i].y);
                vertices.push_back(aimesh->mTangents[i].z);
                vertices.push_back(aimesh->mBitangents[i].x);
                vertices.push_back(aimesh->mBitangents[i].y);
                vertices.push_back(aimesh->mBitangents[i].z);
            }
        }
        
        // INDICES
        if (aimesh->HasFaces()) {
            for (int i = 0; i < aimesh->mNumFaces; i++) {
                aiFace& aiface = aimesh->mFaces[i];
                for (int j = 0; j < aiface.mNumIndices; j++) {
                    indices.push_back(aiface.mIndices[j]);
                }
            }
        }
        
        auto va = Core::Vertex::VertexArray(vertices, vertexAttrFlags);
        auto vbo = std::make_shared<Core::Vbo>(va);
        auto ebo = std::make_shared<Core::Ebo>(indices);
        auto vao = std::make_shared<Core::Vao>(vbo, ebo);
        
        auto mesh = std::make_shared<Mesh>(vao);
        
        const MaterialInfo materialInfo = processMaterial(scene->mMaterials[aimesh->mMaterialIndex]);
        // if (materialInfo.metalness != -1) {
        auto metallicMat = std::make_shared<Material::PBRMetallicMaterial>(materialInfo);
        // }
        mesh->material = metallicMat;
        
        return mesh;
    }

    MaterialInfo ModelNode::processMaterial(aiMaterial* aimaterial) {
        aiShadingMode aishadingmode;
        aimaterial->Get(AI_MATKEY_SHADING_MODEL, aishadingmode);
        
        MaterialInfo result;

        // if (aishadingmode != aiShadingMode_PBR_BRDF) {
        //     // return default material if not PBR (for now)
        //     return result;
        // }

        // Scalars
        aimaterial->Get(AI_MATKEY_METALLIC_FACTOR, result.metalness);
        aimaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, result.roughness);
        aimaterial->Get(AI_MATKEY_GLOSSINESS_FACTOR, result.glossiness);

        // Colors
        aiColor3D aicolor(1.0f, 1.0f, 0.0f);  // Yellow
        aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aicolor);
        result.diffuse = glm::vec3(aicolor.r, aicolor.g, aicolor.b);
        aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, aicolor);
        result.specular = glm::vec3(aicolor.r, aicolor.g, aicolor.b);

        // Textures
        std::vector<aiTextureType> aitextypesToQuery = {aiTextureType_AMBIENT, aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_HEIGHT, aiTextureType_NORMALS, aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_METALNESS, aiTextureType_AMBIENT_OCCLUSION, aiTextureType_LIGHTMAP, aiTextureType_UNKNOWN};
        
        for (const auto& textype : textureTypeMap) {
            if (aimaterial->GetTextureCount(textype.first) > 0) {
                // std::clog << aiTextureTypeToString(textype.first) << " count " << aimaterial->GetTextureCount(textype.first) << std::endl;
                auto texturesLoaded = loadMaterialTextures(aimaterial, textype.first);
                result.textures.insert(result.textures.end(), texturesLoaded.begin(), texturesLoaded.end());
            }
        }

        return result;
    }

    std::vector<std::shared_ptr<Material::Texture>> ModelNode::loadMaterialTextures(aiMaterial* aimaterial, aiTextureType aitextype) {
        std::vector<std::shared_ptr<ImageAsset>> images;
        std::vector<std::shared_ptr<Material::Texture>> textures;
        AssetManager& manager = AssetManager::Instance();

        Material::TextureType textype = textureTypeMap.at(aitextype);

        // aiScene scene;
        // aiString texpath;
        // material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), texpath);
        // // Defines the path of the n’th texture on the stack ‘t’, where ‘n’ is any value >= 0 and ‘t’ is one of the #aiTextureType enumerated values. A file path to an external file or an embedded texture. Use aiScene::GetEmbeddedTexture to test if it is embedded for FBX files, in other cases embedded textures start with ‘*’ followed by an index into aiScene::mTextures.
        // scene.GetEmbeddedTexture(texpath.C_Str());  // for FBX files only
        // scene.mTextures[atoi(texpath.C_Str()+1)];

        // this currently only supports separate texture files, need support for embedded texture files (see above brainstorming)
        for (int i = 0; i < aimaterial->GetTextureCount(aitextype); i++) {            
            aiString str;
            aimaterial->GetTexture(aitextype, i, &str);

            const Path imagePath(head.model->GetFile().Parent().RawPath() / str.C_Str());
            
            // If texture was already loaded from file, just push its existing representation
            bool skip = false;
            for (const auto& texture : head.textures) {
                if (texture->images[0]->GetFile().RawPath() == imagePath.RawPath()
                        && texture->type == textype) {
                    textures.push_back(texture);
                    skip = true;
                    break;
                }
            }
            // Load and push any new textures
            if (!skip) {
                std::clog << "  Loading texture " << imagePath.RawPath() << " as " << aiTextureTypeToString(aitextype) << std::endl;
                auto image = AssetManager::Instance().LoadHot<ImageAsset>(imagePath, false);
                auto texture = std::make_shared<Material::Texture>(image, textype, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
                texture->tex->Bind();
                texture->tex->GenerateMipMap();
                textures.push_back(texture);
                head.textures.push_back(texture);
            }
        }
        return textures;
    }

}
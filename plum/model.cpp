#include <plum/model.hpp>

#include <cassert>
#include <set>
#include <string>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>      // remember: cmake, then make install
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <plum/mesh.hpp>
#include <plum/scenenode.hpp>
#include <plum/shader.hpp>
#include <plum/texture.hpp>

Model::Model(std::string name, std::string path, Shader* default_shader, float scale, GLuint wraptype) 
: Drawable(name, default_shader), Path(path), wrapType(wraptype) {
    // FlipYZ = (path.find(".gltf",name.size()-4) != string::npos);
    std::cout << "  Loading model: " << path << std::endl;
    loadModel(path, scale);
}

Model::~Model() {
    for (auto& texture : texturesLoaded)
        glDeleteTextures(1, &texture->ID);
}

// Methods
void Model::Draw(Shader& shader, glm::mat4 model_matrix) {
    glUseProgram(shader.programID);
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i]->Draw(shader, model_matrix);
    }
}
void Model::DrawInstanced(Shader& shader, std::vector<glm::mat4>& modelMatrices) {
    if (!instancesInitialized) {
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);
        instancesInitialized = true;
    }
    for (int i = 0; i < meshes.size(); i++) {
        meshes[i]->DrawInstanced(shader, modelMatrices.size());
    }
}


void Model::loadModel(std::string path, float scale) {
    Assimp::Importer importer;
    // can also have aiProcess_GenNormals, aiProcess_SplitLargeMeshes, aiProcess_OptimizeMeshes, aiProcess_ValidateDataStructure, ...
    importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, scale);
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_GlobalScale | aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_ValidateDataStructure ); // toilet.3mf needs normals to be generated!
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
    printSceneInfo(path, scene, "../log/scene_info.txt");
};

void Model::processNode(aiNode *node, const aiScene *scene, glm::mat4 parent_model_transformation) {
    // node model transform
    glm::mat4 model_transformation;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            model_transformation[i][j] = node->mTransformation[j][i];
        }
    }
    model_transformation = parent_model_transformation * model_transformation;
    // process node's meshes
    for (int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; // assimp mesh
        meshes.push_back(processMesh(mesh, scene, model_transformation)); // our Mesh
    }
    // recurse into node's children
    for (int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
};

std::shared_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene, glm::mat4 model_transformation) {
    std::vector<Vertex>              vertices;
    std::vector<unsigned int>        indices;
    std::vector<std::shared_ptr<Tex>>     textures;
    std::vector<Mesh::MeshColor>           colors;
    std::vector<Mesh::MeshScalar>          scalars;
    
    // VERTICES
    for (int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // Position
        if (mesh->HasPositions()) {
            vertex.position = glm::vec3(mesh->mVertices[i].x, 
                                        mesh->mVertices[i].y,
                                        mesh->mVertices[i].z);
        }
        // Normal
        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(mesh->mNormals[i].x,
                                        mesh->mNormals[i].y,
                                        mesh->mNormals[i].z);  // mNormals is mNumVertices in size
        }
        // Texture coordinates
        if (mesh->HasTextureCoords(0)) {
            vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x,
                                            mesh->mTextureCoords[0][i].y); // mTextureCoords[x] is mNumVertices in size
        } else {
            vertex.uv = glm::vec2(0);
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent = glm::vec3(mesh->mTangents[i].x,
                                        mesh->mTangents[i].y,
                                        mesh->mTangents[i].z);      
            vertex.bitangent = glm::vec3(mesh->mBitangents[i].x,
                                            mesh->mBitangents[i].y,
                                            mesh->mBitangents[i].z);
        }
        vertices.push_back(vertex);
    }
    
    // INDICES
    for (int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // MATERIAL
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // static int counts[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // for (int i = 0; i < 21; i++) {
    //     counts[i] += material->GetTextureCount(aiTextureType(i));
    //     std::cout << i << ' ' << counts[i] << std::endl;
    // }
    // 1) Textures
    if (material->GetTextureCount(aiTextureType_AMBIENT) > 0) {
        std::vector<std::shared_ptr<Tex>> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, Tex::Tex_Type::TEX_AMBIENT );
        textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        std::vector<std::shared_ptr<Tex>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, Tex::Tex_Type::TEX_DIFFUSE );
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
        std::vector<std::shared_ptr<Tex>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, Tex::Tex_Type::TEX_SPECULAR );
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0) {
        std::vector<std::shared_ptr<Tex>> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, Tex::Tex_Type::TEX_HEIGHT );
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
        std::vector<std::shared_ptr<Tex>> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, Tex::Tex_Type::TEX_NORMAL );
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) {
        std::vector<std::shared_ptr<Tex>> roughnessMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, Tex::Tex_Type::TEX_ROUGHNESS );
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_METALNESS) > 0) {
        std::vector<std::shared_ptr<Tex>> metalnessMaps = loadMaterialTextures(material, aiTextureType_METALNESS, Tex::Tex_Type::TEX_METALNESS );
        textures.insert(textures.end(), metalnessMaps.begin(), metalnessMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0) {
        std::vector<std::shared_ptr<Tex>> occlusionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, Tex::Tex_Type::TEX_OCCLUSION );
        textures.insert(textures.end(), occlusionMaps.begin(), occlusionMaps.end());
    } else if (material->GetTextureCount(aiTextureType_LIGHTMAP) > 0) {    // ambient occlusion for non PBR
        std::vector<std::shared_ptr<Tex>> occlusionMaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, Tex::Tex_Type::TEX_OCCLUSION );
        textures.insert(textures.end(), occlusionMaps.begin(), occlusionMaps.end());
    }
    if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0) {
        std::vector<std::shared_ptr<Tex>> unknownMaps = loadMaterialTextures(material, aiTextureType_UNKNOWN, Tex::Tex_Type::TEX_UNKNOWN );
        textures.insert(textures.end(), unknownMaps.begin(), unknownMaps.end());
    }
    // 2) Colors
    aiColor3D aicolor;
    Mesh::MeshColor color;
    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, aicolor)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_DIFFUSE" << std::endl;
    } else {
        color.type = "color_diffuse";
        color.Value = glm::vec3(aicolor.r, aicolor.g, aicolor.b);
        colors.push_back(color);
    }
    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, aicolor)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_SPECULAR" << std::endl;
    } else {
        color.type = "color_specular";
        color.Value = glm::vec3(aicolor.r, aicolor.g, aicolor.b);
        colors.push_back(color);
    }
    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_AMBIENT, aicolor)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_AMBIENT" << std::endl;
    } else {
        color.type = "color_ambient";
        color.Value = glm::vec3(aicolor.r, aicolor.g, aicolor.b);
        colors.push_back(color);
    }
    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_EMISSIVE, aicolor)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_EMISSIVE" << std::endl;
    } else {
        color.type = "color_emissive";
        color.Value = glm::vec3(aicolor.r, aicolor.g, aicolor.b);
        colors.push_back(color);
    }
    if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_TRANSPARENT, aicolor)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_TRANSPARENT" << std::endl;
    } else {
        color.type = "color_transparent";
        color.Value = glm::vec3(aicolor.r, aicolor.g, aicolor.b);
        colors.push_back(color);
    }
    // 3) Scalars
    float value;
    Mesh::MeshScalar scalar;
    if (AI_SUCCESS != material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_SHININESS" << std::endl;
    } else {
        scalar.type = "roughness";
        scalar.Value = value;
        scalars.push_back(scalar);
    }
    if (AI_SUCCESS != material->Get(AI_MATKEY_METALLIC_FACTOR, value)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_SHININESS" << std::endl;
    } else {
        scalar.type = "metallic";
        scalar.Value = value;
        scalars.push_back(scalar);
    }
    if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, value)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_SHININESS" << std::endl;
    } else {
        scalar.type = "shininess";
        scalar.Value = value;
        scalars.push_back(scalar);
    }
    if (AI_SUCCESS != material->Get(AI_MATKEY_OPACITY, value)) {
        // std::cout << "ERROR::ASSIMP::MAT_GET_OPACITY" << std::endl;
    } else {
        scalar.type = "opacity";
        scalar.Value = value;
        scalars.push_back(scalar);
    }
    
    return std::shared_ptr<Mesh>(new Mesh(vertices, indices, textures, colors, scalars, model_transformation));
};

std::vector<std::shared_ptr<Tex>> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, Tex::Tex_Type textype) {
    std::vector<std::shared_ptr<Tex>> textures;

    // aiScene scene;
    // aiString texpath;
    // material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT, 0), texpath);
    // // Defines the path of the n’th texture on the stack ‘t’, where ‘n’ is any value >= 0 and ‘t’ is one of the #aiTextureType enumerated values. A file path to an external file or an embedded texture. Use aiScene::GetEmbeddedTexture to test if it is embedded for FBX files, in other cases embedded textures start with ‘*’ followed by an index into aiScene::mTextures.
    // scene.GetEmbeddedTexture(texpath.C_Str());  // for FBX files only
    // scene.mTextures[atoi(texpath.C_Str()+1)];

    // this currently only supports separate texture files, need support for embedded texture files (see above brainstorming)
    for (int i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);

        // if texture was already loaded from file, just push its existing representation
        bool skip = false;
        for (auto& texture : texturesLoaded) {
            if (texture->Paths[0] == directory + '/' + std::string(str.C_Str()) && texture->Type == textype) {
                textures.push_back(texture);
                skip = true;
                break;
            }
        }
        // load and push any new textures
        if (!skip) {
            std::shared_ptr<Tex> texture = textureFromFile(str.C_Str(), directory, textype);
            textures.push_back(texture);
            texturesLoaded.push_back(texture);
        }
    }
    return textures;
};

std::shared_ptr<Tex> Model::textureFromFile(std::string path, std::string directory, Tex::Tex_Type textype) {
    std::string filename = directory + "/" + path;
    std::cout << "  ";
    return std::make_shared<Tex>(path, filename, GL_TEXTURE_2D, wrapType, GL_LINEAR, textype);
};

void Model::log() {
    Assimp::Importer importer;
    Assimp::DefaultLogger::create();
    Assimp::DefaultLogger::get()->info(importer.GetErrorString());
    // Assimp::DefaultLogger::get()->attachStream( some LogStream );
    Assimp::DefaultLogger::kill();
}

void Model::printSceneInfo(std::string path, const aiScene *scene, std::string outpath) {            
    // Check if model's scene info was already logged
    if (outpath != "") {
        std::ifstream f(outpath);
        std::string buffer;
        while (getline(f, buffer)) {
            if (buffer.find(path) != std::string::npos) {
                f.close();
                std::cout << "    Scene info already logged in file " << outpath << std::endl;
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
        std::cout << "      " << mat->GetName().C_Str() << std::endl;
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
        // std::cout << "material " << i << " opacity: " << value << std::endl;
        for (int j = 0; j < scene->mMaterials[i]->mNumProperties; j++) {
            materialProperties.insert(scene->mMaterials[i]->mProperties[j]->mKey.C_Str());
        }
    }
    int i = 0;
    for (auto it = materialProperties.begin(); it != materialProperties.end(); it++) {
        ss << "    Material property " << i++ << ":\t" << *it << std::endl;
    }

    // Write to file or std::cout
    if (outpath != "") {
        std::ofstream f(outpath, std::ofstream::app);
        if (f.bad()) {
            std::cout << "Error opening outfile " << outpath << std::endl;
        } else {
            f << ss.rdbuf() << std::endl;
            f.close();
        }
    } else {
        std::cout << ss.rdbuf() << std::endl;
    }
}
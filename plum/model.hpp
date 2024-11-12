#ifndef MODEL_HPP
#define MODEL_HPP

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <assimp/Importer.hpp>      // remember: cmake, then make install
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include <plum/texture.hpp>
#include <plum/scenenode.hpp>

class Shader;
class Mesh;

class Model : public Drawable {
    public:
        std::string Path;

        Model(std::string name, std::string path, Shader* default_shader, float scale = 1.0f, GLuint wraptype = GL_REPEAT);

        ~Model();

        // Methods
        void Draw(Shader& shader, glm::mat4 model_matrix) override;
        void DrawInstanced(Shader& shader, std::vector<glm::mat4>& modelMatrices);
        
    private:
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Tex>> texturesLoaded;
        std::string directory;
        bool instancesInitialized = false;
        GLuint wrapType;

        void loadModel(std::string path, float scale);
        void processNode(aiNode *node, const aiScene *scene, glm::mat4 parent_model_transformation = glm::identity<glm::mat4>());
        std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene, glm::mat4 model_transformation);
        std::vector<std::shared_ptr<Tex>> loadMaterialTextures(aiMaterial *material, aiTextureType type, Tex::Tex_Type textype);
        std::shared_ptr<Tex> textureFromFile(std::string path, std::string directory, Tex::Tex_Type textype);

        void log();
        void printSceneInfo(std::string path, const aiScene *scene, std::string outpath = "");
};

#endif
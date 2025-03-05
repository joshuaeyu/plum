#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <map>

#include <glad/gl.h>

#include <glm/glm.hpp>

#include <plum/camera.hpp>
#include <plum/light.hpp>
#include <plum/model.hpp>
#include <plum/resources.hpp>
#include <plum/scenenode.hpp>
#include <plum/shape.hpp>

class Light;
class DirectionalLight;
class PointLight;
class SpotLight;

class Scene {
    public:
        std::vector<std::shared_ptr<DirectionalLight>> DirLights;
        std::vector<std::shared_ptr<PointLight>> PointLights;
        std::vector<std::shared_ptr<SpotLight>> SpotLights;
        std::vector<std::shared_ptr<SceneNode>> Nodes;
        std::shared_ptr<Tex> EnvironmentMap;

        Camera SceneCamera;
        
        // Need logic/structure for forward rendering

        Scene();

        void DrawNodes();
        void DrawNodes(Shader& shader);
        void DrawLights();
        void DrawLights(Shader &shader);

        std::shared_ptr<DirectionalLight> CreateDirectionalLight();
        std::shared_ptr<DirectionalLight> CreateDirectionalLight(DirectionalLight light);

        std::shared_ptr<PointLight> CreatePointLight();
        std::shared_ptr<PointLight> CreatePointLight(PointLight light);

        std::shared_ptr<SpotLight> CreateSpotLight();
        std::shared_ptr<SpotLight> CreateSpotLight(SpotLight light);

        std::shared_ptr<SceneNode> CreateNode(std::shared_ptr<Drawable> node_template);

        void Delete(std::shared_ptr<DirectionalLight> dirlight);
        void Delete(std::shared_ptr<PointLight> pointlight);
        void Delete(std::shared_ptr<SceneNode> node);

        void InitializeUniformBlocks();

        void SetVertexMatricesUniformBlock();
        void SetFragmentMatricesUniformBlock();
        void SetCameraUniformBlock();
        void SetPointLightUniformBlock();
        void SetDirectionalLightUniformBlock();

    private:
        GLuint ubo_matrices_vs;
        GLuint ubo_matrices_fs;
        GLuint ubo_camera_fs;
        GLuint ubo_dirlight_fs;
        GLuint ubo_pointlight_fs;
};

#endif
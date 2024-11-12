#include <plum/scene.hpp>

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

Scene::Scene() {
    SceneCamera = Camera(glm::vec3(-11,5,4),0,-30);
}

void Scene::DrawNodes() {
    for (auto& node : Nodes) {
        node->DrawDefault();
    }
}
void Scene::DrawNodes(Shader& shader) {
    for (auto& node : Nodes) {
        node->Draw(shader);
    }
}
void Scene::DrawLights() {
    for (auto& pl : PointLights) {
        pl->DrawDefault();
    }
}
void Scene::DrawLights(Shader &shader) {
    for (auto& pl : PointLights) {
        pl->Draw(shader);
    }
}

std::shared_ptr<DirectionalLight> Scene::CreateDirectionalLight() {
    std::shared_ptr<DirectionalLight> dl(new DirectionalLight);
    DirLights.push_back(dl);
    return dl;
}
std::shared_ptr<DirectionalLight> Scene::CreateDirectionalLight(DirectionalLight light) {
    std::shared_ptr<DirectionalLight> dl(new DirectionalLight(light));
    DirLights.push_back(dl);
    return dl;
}

std::shared_ptr<PointLight> Scene::CreatePointLight() {
    std::shared_ptr<PointLight> pl(new PointLight);
    PointLights.push_back(pl);
    return pl;
}
std::shared_ptr<PointLight> Scene::CreatePointLight(PointLight light) {
    std::shared_ptr<PointLight> pl(new PointLight(light));
    PointLights.push_back(pl);
    return pl;
}

std::shared_ptr<SpotLight> Scene::CreateSpotLight() {
    std::shared_ptr<SpotLight> pl(new SpotLight);
    SpotLights.push_back(pl);
    return pl;
}
std::shared_ptr<SpotLight> Scene::CreateSpotLight(SpotLight light) {
    std::shared_ptr<SpotLight> sl(new SpotLight(light));
    SpotLights.push_back(sl);
    return sl;
}

std::shared_ptr<SceneNode> Scene::CreateNode(std::shared_ptr<Drawable> node_template) {
    std::shared_ptr<SceneNode> node(new SceneNode(node_template));
    Nodes.push_back(node);
    return node;
}

void Scene::Delete(std::shared_ptr<DirectionalLight> dirlight) {
    auto it = find(DirLights.begin(), DirLights.end(), dirlight);
    DirLights.erase(it);
}
void Scene::Delete(std::shared_ptr<PointLight> pointlight) {
    auto it = find(PointLights.begin(), PointLights.end(), pointlight);
    PointLights.erase(it);
}
void Scene::Delete(std::shared_ptr<SceneNode> node) {
    auto it = find(Nodes.begin(), Nodes.end(), node);
    Nodes.erase(it);
}

void Scene::InitializeUniformBlocks() {
    // 0 - View, projection transforms
    glGenBuffers(1, &ubo_matrices_vs);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_vs);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_matrices_vs);  // 0 is the binding point for this uniform block
    
    // 1 - Inverse view transform
    glGenBuffers(1, &ubo_matrices_fs);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_fs);
    glBufferData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_matrices_fs);  // 1 is the binding point for this uniform block

    // 2 - Camera position, front (viewspace)
    glGenBuffers(1, &ubo_camera_fs);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_camera_fs);
    glBufferData(GL_UNIFORM_BUFFER, 2 * 16, NULL, GL_DYNAMIC_DRAW); // Note 4N byte alignment.
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo_camera_fs);  // 2 is the binding point for this uniform block
    
    // 3 - Directional light colors, direction, lightspace transform
    glGenBuffers(1, &ubo_dirlight_fs);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_dirlight_fs);
    glBufferData(GL_UNIFORM_BUFFER, 16 + 8 * (16+16 + sizeof(glm::mat4)), NULL, GL_DYNAMIC_DRAW); // Note 4N byte alignment.
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, ubo_dirlight_fs);  // 3 is the binding point for this uniform block

    // 4 - Point light count, colors, attenuations, positions, positions (worldspace)
    glGenBuffers(1, &ubo_pointlight_fs);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_pointlight_fs);
    glBufferData(GL_UNIFORM_BUFFER, 16 + 32 * (4*16), NULL, GL_DYNAMIC_DRAW); // Note 4N byte alignment.
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 4, ubo_pointlight_fs);  // 4 is the binding point for this uniform block
}

void Scene::SetVertexMatricesUniformBlock() {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_vs);
    glm::mat4 view = SceneCamera.GetViewMatrix();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &view);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &SceneCamera.Projection);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Scene::SetFragmentMatricesUniformBlock() {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices_fs);
    glm::mat4 inv_view = glm::inverse(SceneCamera.GetViewMatrix());
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &inv_view);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Scene::SetCameraUniformBlock() {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_camera_fs);
    glm::vec3 dummy_cam_pos = glm::vec3(0); // shaders work in view space
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &dummy_cam_pos);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), &SceneCamera.Front);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Scene::SetPointLightUniformBlock() {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_pointlight_fs);

    float pointlight_count = PointLights.size();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &pointlight_count); // Note 4N byte alignment.

    int shadow_count = 0;
    for (int i = 0; i < pointlight_count; i++) {
        GLintptr offset = 16 + i*64;

        glm::vec4 pointlight_color = glm::vec4(PointLights[i]->Color, 0);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4), &pointlight_color);  // Note 4N byte alignment.

        float pointlight_att[] = { PointLights[i]->GetAttenuationConstant(), PointLights[i]->GetAttenuationLinear(), PointLights[i]->GetAttenuationQuadratic() };
        glm::vec4 att = glm::vec4(pointlight_att[0], pointlight_att[1], pointlight_att[2], 0);
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, sizeof(glm::vec4), &att);  // Note 4N byte alignment.

        glm::vec4 pointlight_pos_view = SceneCamera.GetViewMatrix() * glm::vec4(PointLights[i]->Position, 1);
        pointlight_pos_view.w = PointLights[i]->GetFarPlane();
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, sizeof(glm::vec4), &pointlight_pos_view);

        glm::vec4 pointlight_pos_world = glm::vec4(PointLights[i]->Position, -1);
        if (PointLights[i]->HasShadows()) {
            pointlight_pos_world = glm::vec4(PointLights[i]->Position, shadow_count++);
        }
        glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, sizeof(glm::vec4), &pointlight_pos_world);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Scene::SetDirectionalLightUniformBlock() {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_dirlight_fs);

    float dirlight_count = DirLights.size();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &dirlight_count); // Note 4N byte alignment.

    int shadow_count = 0;
    for (int i = 0; i < dirlight_count; i++) {
        GLintptr offset = 16 + i*96;

        glm::vec4 dirlight_color = glm::vec4(DirLights[i]->Color, 0);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4), &dirlight_color);  // Note 4N byte alignment.

        glm::mat4 view = SceneCamera.GetViewMatrix();
        if (DirLights[i]->HasShadows()) {
            glm::vec4 dirlight_direction = 
                glm::vec4( glm::mat3(glm::transpose(glm::inverse(view))) * DirLights[i]->Direction, shadow_count++ );
            glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, sizeof(glm::vec4), &dirlight_direction);
            
            glm::mat4 dirlight_matrix = DirLights[i]->GetLightspaceMatrix();
            glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, sizeof(glm::mat4), &dirlight_matrix); // Note 4N byte alignment.
        } else {
            glm::vec4 dirlight_direction = 
                glm::vec4( glm::mat3(glm::transpose(glm::inverse(view))) * DirLights[i]->Direction, -1 );
            glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, sizeof(glm::vec4), &dirlight_direction);

            // Don't need to set lightspace matrix if not casting shadows
        }

    }

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

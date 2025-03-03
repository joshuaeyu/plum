#include <plum/material/module.hpp>

namespace Material {

    DirectionalShadowModule::DirectionalShadowModule() {}

    void DirectionalShadowModule::SetGlobalUniforms(Component::DirectionalLight& dl, const GLuint depthTexture, int* shadowIdx) {
        if (!dl.HasShadows())
            return;

        program->SetMat4("lightSpaceMatrix", dl.GetLightspaceMatrix());
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0, *(shadowIdx)++);
    }
    void DirectionalShadowModule::SetObjectUniforms(const glm::mat4& model) {
        program->SetMat4("model", model);
    }
    void DirectionalShadowModule::Use() {
        program->Use();
    }
    std::shared_ptr<Program> DirectionalShadowModule::GetProgram() {
        return program;
    }

    PointShadowModule::PointShadowModule() {}

    void PointShadowModule::SetGlobalUniforms(Component::PointLight& pl, const glm::vec3& pos, int* shadowIdx) {
        if (!pl.HasShadows()) {
            return;
        program->SetInt("layer", *(shadowIdx)++);
        for (int j = 0; j < 6; j++) {
            program->SetMat4("lightSpaceMatrices[" + std::to_string(j) + "]", (pl.GetLightspaceMatrices())[j]);
        }
        program->SetVec3("lightPos", pos);
        program->SetFloat("far", pl.GetFarPlane());
        }
    }
    void PointShadowModule::SetObjectUniforms(const glm::mat4& model) {
        program->SetMat4("model", model);
    }
    void PointShadowModule::Use() {
        program->Use();
    }
    std::shared_ptr<Program> PointShadowModule::GetProgram() {
        return program;
    }

    LightingPassPBRModule::LightingPassPBRModule() {}

    void LightingPassPBRModule::SetGlobalUniforms() {
        program->SetInt("gPosition", gPosition);
        program->SetInt("gNormal", gNormal);
        program->SetInt("gAlbedoSpec", gAlbedoSpec);
        program->SetInt("gMetRouOcc", gMetRouOcc);

        program->SetInt("irradianceMap", irradianceMap);
        program->SetInt("prefilterMap", prefilterMap);
        program->SetInt("brdfLUT", brdfLUT);
        program->SetFloat("ibl", ibl);

        program->SetInt("shadowmap_2d_array_shadow", shadowmap_2d_array_shadow);
        program->SetInt("shadowmap_cube_array_shadow", shadowmap_cube_array_shadow);
    }
    void LightingPassPBRModule::Use() {
        program->Use();
    }
    std::shared_ptr<Program> LightingPassPBRModule::GetProgram() {
        return program;
    }

    SkyboxModule::SkyboxModule() {}
    
    void SkyboxModule::SetGlobalUniforms(const glm::mat4& view, const glm::mat4& projection, const int cubemapUnit) {
        if (hdr) {
            hdrProgram->SetMat4("view", view);
            hdrProgram->SetMat4("projection", projection);
            hdrProgram->SetInt("cubemap", cubemapUnit);

        } else {
            program->SetMat4("view", view);
            program->SetMat4("projection", projection);
            program->SetInt("cubemap", cubemapUnit);
        }
    }
    void SkyboxModule::Use() {
        if (hdr)
            hdrProgram->Use();
        else
            program->Use();
    }
    std::shared_ptr<Program> SkyboxModule::GetProgram() {
        if (hdr)
            return hdrProgram;
        else
            return program;
    }

}
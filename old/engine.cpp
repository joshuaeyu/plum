#include <plum/engine.hpp>

#include <iostream>
#include <vector>
#include <random>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plum/shader.hpp>
#include <plum/shape.hpp>
#include <plum/texture.hpp>

Engine::Engine() {}

// Sets screen dimensions for use in other Engine calls. Default screen dimensions are 1920x1080.
// Screen width and height are used in InitGbuffer, InitSsao, InitHdr, InitBloom, InitFxaa.
void Engine::SetScreenDimensions(const unsigned int width, const unsigned int height) {
    ScreenWidth = width;
    ScreenHeight = height;
}

// Renders a quad to the entire screen. Generally used for postprocessing.
// User must call glUseProgram and set fragment shader uniforms prior to calling.
void Engine::RenderQuad() {
    if (!quadInitiaized) {
        float quadVertices[] = {    // Remember CCW
            -1.0f, -1.0f,   0.0f, 0.0f,
            1.0f, -1.0f,   1.0f, 0.0f,
            -1.0f,  1.0f,   0.0f, 1.0f,

            1.0f,  1.0f,   1.0f, 1.0f,
            -1.0f,  1.0f,   0.0f, 1.0f,
            1.0f, -1.0f,   1.0f, 0.0f
        };
        GLuint quadVBO;
        glGenBuffers(1, &quadVBO);
        glGenVertexArrays(1, &quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glBindVertexArray(quadVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        quadInitiaized = true;
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// Render an equirectangular texture to a cubemap texture. For environment cubemapping.
// - equirect: equirectangular texture to render
// - shader: equirectangular-to-cubemap shader
GLuint Engine::_equirectToCubemap(Tex& equirect, Shader& shader) {
    const int res = 1024;

    GLuint fbo, depth;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res, res);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    GLuint cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, res, res, GL_FALSE, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glm::mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f);
    glm::mat4 views[] = {
        glm::lookAt(glm::vec3(0), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glUseProgram(shader.programID);
    shader.setInt("equirectMap", 0);
    shader.setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, equirect.ID);
    
    Cube unitcube("cube");

    glViewport(0, 0, res, res);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (int i = 0; i < 6; i++) {
        shader.setMat4("view", views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
            exit(1);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        unitcube.Draw(shader);
        glCullFace(GL_BACK);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);  // To prevent dotted artifacts for certain environments during prefilter convolution
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    return cubemap;
}

// Generates an irradiance cubemap from the given (environmental) cubemap. For diffuse IBL.
// - envcubemap: environmental cubemap
// - shader: cubemap-to-irradiance shader
GLuint Engine::_envToIrradiance(GLuint envcubemap, Shader& shader) {
    const int res = 64;
    
    GLuint fbo, depth;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res, res);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    GLuint irrcubemap;
    glGenTextures(1, &irrcubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irrcubemap);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, res, res, GL_FALSE, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f);
    glm::mat4 views[] = {
        glm::lookAt(glm::vec3(0), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glUseProgram(shader.programID);
    shader.setInt("envCubemap", 0);
    shader.setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envcubemap);
    
    Cube unitcube("cube");

    glViewport(0, 0, res, res);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (int i = 0; i < 6; i++) {
        shader.setMat4("view", views[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irrcubemap, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
            exit(1);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        unitcube.Draw(shader);
        glCullFace(GL_BACK);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return irrcubemap;
}

// Generates a prefiltered cubemap from the given (environmental) cubemap. For specular IBL.
// - envcubemap: environmental cubemap
// - shader: cubemap prefilter shader
GLuint Engine::_envToPrefilter(GLuint envcubemap, Shader& shader) {
    const int env_res = 1024;
    const int res = 512;

    GLuint fbo, depth;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res, res);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    GLuint prefilterCubemap;
    glGenTextures(1, &prefilterCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemap);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, res, res, GL_FALSE, GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glm::mat4 projection = glm::perspective(glm::half_pi<float>(), 1.0f, 0.1f, 10.0f);
    glm::mat4 views[] = {
        glm::lookAt(glm::vec3(0), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glUseProgram(shader.programID);
    shader.setInt("envCubemap", 0);
    shader.setMat4("projection", projection);
    shader.setInt("envResolution", env_res);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envcubemap);
    
    Cube unitcube("unitcube");

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    int maxMipLevels = 5;
    for (int mip = 0; mip < maxMipLevels; mip++) {

        int mipRes = res * std::pow(0.5, mip);

        glBindRenderbuffer(GL_RENDERBUFFER, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipRes, mipRes);
        glViewport(0, 0, mipRes, mipRes);
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        shader.setFloat("roughness", roughness);

        for (int i = 0; i < 6; i++) {
            shader.setMat4("view", views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterCubemap, mip);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
                exit(1);
            }
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glCullFace(GL_FRONT);
            unitcube.Draw(shader);
            glCullFace(GL_BACK);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return prefilterCubemap;
}

void Engine::InitEnvironment(std::shared_ptr<Tex> map) {
    CurrentEnvironment = Environment();
    if (map->Target == GL_TEXTURE_2D) {
        CurrentEnvironment.environment = _equirectToCubemap(*map, *EquirectShader);
        CurrentEnvironment.from2D = true;
    }
    else if (map->Target == GL_TEXTURE_CUBE_MAP)
        CurrentEnvironment.environment = map->ID;
    CurrentEnvironment.irradiance = _envToIrradiance(CurrentEnvironment.environment, *IrradianceShader);
    CurrentEnvironment.prefilter = _envToPrefilter(CurrentEnvironment.environment, *PrefilterShader);
}

// Generates the BRDF lookup texture. For specular IBL.
// - shader: BRDF integration shader
GLuint Engine::GenerateBrdfLut(Shader& shader) {
    const int res = 512;

    GLuint fbo, depth;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res, res);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    GLuint brdfLUT;
    glGenTextures(1, &brdfLUT);
    glBindTexture(GL_TEXTURE_2D, brdfLUT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, res, res, GL_FALSE, GL_RG, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    }
    
    glViewport(0,0,res,res);
    glUseProgram(shader.programID);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return brdfLUT;
}

// Generates a framebuffer with a texture array as its depth attachment. The texture array is a 3D texture containing 8 layers of depth-only 2D or cubemap textures.
// Uses Engine::ShadowWidth2D and Engine::ShadowHeight2D, or Engine::ShadowWidthCube and Engine::ShadowHeightCube, depending on target.
// - target: GL_TEXTURE_2D_ARRAY or GL_TEXTURE_CUBE_MAP_ARRAY
Engine::Framebuffer Engine::GenerateShadowMapArray(GLenum target) {
    Framebuffer result;

    glGenFramebuffers(1, &result.fbo);
    glGenTextures(1, &result.depth);

    glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);

    glBindTexture(target, result.depth);
    glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // faster than GL_LINEAR
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // faster than GL_LINEAR
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    int num_layers;
    if (target == GL_TEXTURE_CUBE_MAP_ARRAY) {
        num_layers = 6 * 8;
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexImage3D(target, 0, GL_DEPTH_COMPONENT16, ShadowWidthCube, ShadowHeightCube, num_layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    } else if (target == GL_TEXTURE_2D_ARRAY) {
        num_layers = 8;
        float border_color[] = {1.f, 1.f, 1.f, 1.f};
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border_color);
        glTexImage3D(target, 0, GL_DEPTH_COMPONENT16, ShadowWidth2D, ShadowHeight2D, num_layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, result.depth, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return result;
}

// Initialize G-buffer framebuffer, Gbuffer.
void Engine::InitGbuffer() {

    if (!Gbuffer.fbo) {
        glGenFramebuffers(1, &Gbuffer.fbo);
        Gbuffer.colors = std::vector<GLuint>(4);
        glGenTextures(4, &Gbuffer.colors[0]);
        glGenRenderbuffers(1, &Gbuffer.depth);

        glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer.fbo);
        GLenum attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, attachments);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer.fbo);

    GLuint &position = Gbuffer.colors[0];
    // glGenTextures(1, position);
    glBindTexture(GL_TEXTURE_2D, position);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position, 0);

    GLuint &normal = Gbuffer.colors[1];
    // glGenTextures(1, normal);
    glBindTexture(GL_TEXTURE_2D, normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
    
    GLuint &albedospec = Gbuffer.colors[2];
    // glGenTextures(1, albedospec);
    glBindTexture(GL_TEXTURE_2D, albedospec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedospec, 0);
    
    GLuint &metrou = Gbuffer.colors[3];
    // glGenTextures(1, metrou);
    glBindTexture(GL_TEXTURE_2D, metrou);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, metrou, 0);

    GLuint &depth = Gbuffer.depth; 
    // glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, ScreenWidth, ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    }
}

// Initialize SSAO kernel SsaoKernel, noise texture SsaoNoiseTexture, and framebuffers Ssao and SsaoBlur.
// void InitSsao(GLuint* noise, GLuint* fbo, GLuint* color, GLuint* blurfbo, GLuint* blurcolor) {
void Engine::InitSsao() {

    if (!Ssao.fbo || !SsaoBlur.fbo) {
        // Generate sampling kernel (random vectors in tangent space in the +normal hemisphere)
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        for (int i = 0; i < 64; i++) {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = randomFloats(generator) * glm::normalize(sample);
            float scale = (float)i / 64.0;
            scale = 0.1 + scale*scale * (1.0 - 0.1);    // lerp, places more samples closer to origin of hemisphere
            sample *= scale;
            SsaoKernel.push_back(sample);
        }

        // Generate noise texture (random vectors in tangent space on the tangent-bitangent plane)
        std::vector<glm::vec3> noiseVector;    
        for (int i = 0; i < 16; i++) {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0);
            noiseVector.push_back(sample);
        }
        glGenTextures(1, &SsaoNoiseTexture);
        glBindTexture(GL_TEXTURE_2D, SsaoNoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, GL_FALSE, GL_RGB, GL_FLOAT, noiseVector.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenFramebuffers(1, &Ssao.fbo);
        Ssao.colors = std::vector<GLuint>(1);
        glGenTextures(1, &Ssao.colors[0]);
        
        glGenFramebuffers(1, &SsaoBlur.fbo);
        SsaoBlur.colors = std::vector<GLuint>(1);
        glGenTextures(1, &SsaoBlur.colors[0]);
    }

    // Base FBO
    glBindFramebuffer(GL_FRAMEBUFFER, Ssao.fbo);
    glBindTexture(GL_TEXTURE_2D, Ssao.colors[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ScreenWidth, ScreenHeight, GL_FALSE, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Ssao.colors[0], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    }
    
    // Blur FBO
    glBindFramebuffer(GL_FRAMEBUFFER, SsaoBlur.fbo);
    glBindTexture(GL_TEXTURE_2D, SsaoBlur.colors[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ScreenWidth, ScreenHeight, GL_FALSE, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SsaoBlur.colors[0], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    } 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Initialize HDR tone mapping framebuffer, Hdr.
void Engine::InitHdr() {

    if (!Hdr.fbo) {
        glGenFramebuffers(1, &Hdr.fbo);
        Hdr.colors = std::vector<GLuint>(1);
        glGenTextures(1, &Hdr.colors[0]);
        glGenRenderbuffers(1, &Hdr.depth);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, Hdr.fbo);
    
    GLuint &color = Hdr.colors[0];
    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    GLuint &depth = Hdr.depth; 
    glGenRenderbuffers(1, &depth);
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, ScreenWidth, ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Initialize Bloom framebuffers, Bloom1 and Bloom2.
void Engine::InitBloom() {
    
    if (!Bloom1.fbo || !Bloom2.fbo) {
        glGenFramebuffers(1, &Bloom1.fbo);
        Bloom1.colors = std::vector<GLuint>(2);
        glGenTextures(2, &Bloom1.colors[0]);
        glGenRenderbuffers(1, &Bloom1.depth);

        glGenFramebuffers(1, &Bloom2.fbo);
        Bloom2.colors = std::vector<GLuint>(2);
        glGenTextures(2, &Bloom2.colors[0]);

        GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glBindFramebuffer(GL_FRAMEBUFFER, Bloom1.fbo);
        glDrawBuffers(2, attachments);
        glBindFramebuffer(GL_FRAMEBUFFER, Bloom2.fbo);
        glDrawBuffers(2, attachments);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, Bloom1.fbo);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, Bloom1.colors[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, Bloom1.colors[i], 0);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, Bloom1.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, ScreenWidth, ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Bloom1.depth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, Bloom2.fbo);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, Bloom2.colors[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, Bloom2.colors[i], 0);
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    }   

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Initialize FXAA framebuffer, Fxaa.
void Engine::InitFxaa() {
    
    if (!Fxaa.fbo) {
        glGenFramebuffers(1, &Fxaa.fbo);
        Fxaa.colors = std::vector<GLuint>(1);
        glGenTextures(1, &Fxaa.colors[0]);
        glGenRenderbuffers(1, &Fxaa.depth);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, Fxaa.fbo);
    
    GLuint &color = Fxaa.colors[0];
    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ScreenWidth, ScreenHeight, GL_FALSE, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

    GLuint &depth = Fxaa.depth; 
    glBindRenderbuffer(GL_RENDERBUFFER, depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, ScreenWidth, ScreenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "framebuffer error: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
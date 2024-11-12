#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <glad/gl.h>
#include <glm/glm.hpp>

class Shader;
class Tex;

class Engine {

    public:

        unsigned int ScreenWidth        = 1920;
        unsigned int ScreenHeight       = 1080;
        unsigned int ShadowWidth2D      = 2048;
        unsigned int ShadowHeight2D     = 2048;
        unsigned int ShadowWidthCube    = 1024;
        unsigned int ShadowHeightCube   = 1024;

        GLuint quadVAO;
        bool quadInitiaized = false;

        Shader *EquirectShader, *IrradianceShader, *PrefilterShader;

        struct Framebuffer {
            GLuint fbo = 0;
            std::vector<GLuint> colors;
            GLuint depth;
        };

        struct Environment {
            bool from2D = true;
            GLuint environment, irradiance, prefilter;
            ~Environment() {
                if (from2D)
                    glDeleteTextures(1, &environment);
                glDeleteTextures(1, &irradiance);
                glDeleteTextures(1, &prefilter);
            }
        };
        
        // G-buffer framebuffer. Call InitGbuffer() before use.
        // - Gbuffer.colors[0]: position
        // - Gbuffer.colors[1]: normal
        // - Gbuffer.colors[2]: albedo, specular
        // - Gbuffer.colors[3]: metallic, roughness
        Framebuffer Gbuffer;
        
        // SSAO framebuffers, sampling kernel, and noise texture. Call InitSsao() before use.
        // - Ssao.colors[0]: occluded pixels
        // - SsaoBlur.colors[0]: occluded pixels, blurred
        Framebuffer Ssao, SsaoBlur;
        // SSAO sampling kernel. Call InitSsao() before use.
        std::vector<glm::vec3> SsaoKernel;
        // SSAO noise texture. Call InitSsao() before use.
        GLuint SsaoNoiseTexture;

        // HDR framebuffer. Call InitHdr() before use.
        // - Hdr.colors[0]: HDR tone mapped screen
        Framebuffer Hdr;

        // Bloom framebuffers. Call InitBloom() before use.
        // - Bloom1.colors[0]: raw pixels
        // - Bloom1.colors[1]: bright pixels, blurred
        // - Bloom2.colors[0]: dummy attachment
        // - Bloom2.colors[1]: bright pixels, blurred
        Framebuffer Bloom1, Bloom2;

        // FXAA framebuffer. Call InitFxaa() before use.
        // - Fxaa.colors[0]: antialiased screen
        Framebuffer Fxaa;
        
        // Current environment cubemap and supporting cubemaps for PBR.
        Environment CurrentEnvironment;


        Engine();

        // Sets screen dimensions for use in other Engine calls. Default screen dimensions are 1920x1080.
        // Screen width and height are used in InitGbuffer, InitSsao, InitHdr, InitBloom, InitFxaa.
        void SetScreenDimensions(const unsigned int width, const unsigned int height);

        // Renders a quad to the entire screen. Generally used for postprocessing.
        // User must call glUseProgram and set fragment shader uniforms prior to calling.
        void RenderQuad();

        // Render an equirectangular texture to a cubemap texture. For environment cubemapping.
        // - equirect: equirectangular texture to render
        // - shader: equirectangular-to-cubemap shader
        GLuint _equirectToCubemap(Tex& equirect, Shader& shader);

        // Generates an irradiance cubemap from the given (environmental) cubemap. For diffuse IBL.
        // - envcubemap: environmental cubemap
        // - shader: cubemap-to-irradiance shader
        GLuint _envToIrradiance(GLuint envcubemap, Shader& shader);

        // Generates a prefiltered cubemap from the given (environmental) cubemap. For specular IBL.
        // - envcubemap: environmental cubemap
        // - shader: cubemap prefilter shader
        GLuint _envToPrefilter(GLuint envcubemap, Shader& shader);

        void InitEnvironment(std::shared_ptr<Tex> map);

        // Generates the BRDF lookup texture. For specular IBL.
        // - shader: BRDF integration shader
        GLuint GenerateBrdfLut(Shader& shader);

        // Generates a framebuffer with a texture array as its depth attachment. The texture array is a 3D texture containing 8 layers of depth-only 2D or cubemap textures.
        // Uses Engine::ShadowWidth2D and Engine::ShadowHeight2D, or Engine::ShadowWidthCube and Engine::ShadowHeightCube, depending on target.
        // - target: GL_TEXTURE_2D_ARRAY or GL_TEXTURE_CUBE_MAP_ARRAY
        Framebuffer GenerateShadowMapArray(GLenum target);

        // Initialize G-buffer framebuffer, Gbuffer.
        void InitGbuffer();

        // Initialize SSAO kernel SsaoKernel, noise texture SsaoNoiseTexture, and framebuffers Ssao and SsaoBlur.
        // void InitSsao(GLuint* noise, GLuint* fbo, GLuint* color, GLuint* blurfbo, GLuint* blurcolor) {
        void InitSsao();

        // Initialize HDR tone mapping framebuffer, Hdr.
        void InitHdr();

        // Initialize Bloom framebuffers, Bloom1 and Bloom2.
        void InitBloom();

        // Initialize FXAA framebuffer, Fxaa.
        void InitFxaa();

};

#endif
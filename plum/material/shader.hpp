#pragma once

#include <plum/material/material.hpp>

namespace Material {

    class Shader {

        public:

            // does material/shader or renderer set or update uniforms
            // renderer?
            // node->obj->setuniforms?
            // renderer needs to go in an parse object/material information
            
            // universal: 
            
            // userShader: PBR, Phong, wireframe, ...
            //  - defines STRUCTS/FIELDS and SHADING CALCULATIONS
            //  - how materials, lights, camera are used
            //  - layout of Material, Light uniform structs
            //  - shader must be privvy to Material object

            //  - things like ibl, irradiance, prefilter, brdfLUT
            //     - userShader needs access to skybox, which in some Environment class?
            //  - shadow maps, ssao, any non-material textures
                // - where is this stuff stored/updated - renderer?

            // material: PBR, Phong, Wireframe, ... (i.e., userShader instance)
            //  - determines FIELD VALUES
            //  - values within the Material uniform structs
            //  - fields available are determined by the associated shader.
            //  - material can be supported by multiple shaders?
            //  - "read only"

            // shader
            //  - for any purpose (vertex shading, frag shading, geo shading, postprocessing, ...)
            //  - need to distinguish shaderfile -> shader program vs content of shader itself - i.e., material
            // program
            //  - shaders put together

            // renderer: forward, deferred, tiled, forward+, ...
            //  - BUILDS SHADER PROGRAMS for each unique material by COMBINING STRUCTS/FIELDS/CALCS
                //  - which shader stage(s) Material structs are located
                //  - how data are passed between shader stages
                //  - vertex shader(?) and camera are independent of materials
            //  - RENDERS SCENES
                //  - Renderer::Renderer()
                //  - Renderer::Initialize(Scene s) {
                //      for each node in scene whose material needs a shaderprogram
                //          check if shaderprogram for its material exists. if not:
                    //          build shaderprogram for its material
                    //      populate material's shaderprogram pointer
                // 
                //                 
                //  }
                //  - Framebuffer Renderer::Render(Scene s) {

                //      set universal uniforms // shaders need to "subscribe" by binding their ubos appropriately - renderer can do this for the shader
                //      for each node in scene
                //          if shaderprogram exists, draw node
                //          else build shader program, then draw node
                //      render skybox
                //      return final Framebuffer for postprocessing to usesdfsds
                // {
                // builds shaders on the first render call
            //  - for frag shading: sets uniforms using some interface set by the shader?
            //  - for vertex shading: does it directly
            //  - some uniforms determined by renderer itself
            //  - universal data: camera, lights* (lights can be prefiltered by the renderer, i.e., to only choose nearby lights)

            // postprocessing: bloom, HDR, AA


            // pipeline

            // Initialization
            // for each shader:
            // Shader::SetUpUniformBlocks();
            // Renderer::BuildProgram(shader);
            //
            // Rendering (per-shader globals)
            // anything constant throughout draw calls like Light
            // need to inventory shaders and globals
            // globals are determined by shaders
            // scene needs all shaders used? or all shaders used are stored in some global lookup table?
            //
            // Rendering (meshes)
            // Renderer::SetUniforms() {
                // for each node:
                    // if mesh:
                        // node->material->shader->set
            // }
            // Renderer::Draw() {
                // draw each node
            // }
            // 

            // render(scene)
    // iterate through scene graph and set scene-wide? uniforms
    // iterate through scene graph and call draw
        // use shader associated with material
        // set object-specific uniforms
        // draw

            const GLuint programID;

            Shader(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath = "");
            Shader(std::string vertexShaderCode, std::string fragmentShaderCode, std::string geometryShaderCode = "");

            ~Shader();

            // currently defined in the renderer
            virtual void SetUniformBlockBinding();

            virtual void SetUniforms(const glm::mat4 model_matrix, const Material& material) const;

            void setUniformBlockBinding(std::string name, GLuint index);

            void setInt(std::string name, int val);
            void setFloat(std::string name, float val);
            void setVec3(std::string name, glm::vec3 vec);
            void setMat4(std::string name, glm::mat4 mat);

            void Use();

    };

    class PBRMetallicShader : public Shader {

        void SetUniformBlockBinding();


    };

}
#pragma once

#include <plum/asset/manager.hpp>
#include <plum/asset/shader.hpp>

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Core {

    class Program : public AssetUser {

        public:
            // May implement non-default UBO schemes in the future
            enum class UboScheme {
                Scheme1
            };

            Program(std::shared_ptr<ShaderAsset> vert_shader, std::shared_ptr<ShaderAsset> frag_shader, std::shared_ptr<ShaderAsset> geom_shader = nullptr);
            // Program(const std::string& vertexShaderCode, const std::string& fragmentShaderCode, const std::string& geometryShaderCode = "");

            GLuint Handle() const { return handle; }

            void SetUniformBlockBindingScheme(UboScheme scheme = UboScheme::Scheme1);
            void SetUniformBlockBinding(const std::string& name, GLuint index);

            void SetInt(const std::string& name, int val);
            void SetFloat(const std::string& name, float val);
            void SetVec3(const std::string& name, const glm::vec3& vec);
            void SetMat4(const std::string& name, const glm::mat4& mat);

            void Use();
            
            // Asset
            void AssetResyncCallback() override;

            std::shared_ptr<ShaderAsset> vertexShader;
            std::shared_ptr<ShaderAsset> fragmentShader;
            std::shared_ptr<ShaderAsset> geometryShader;
        
        private:
            GLuint handle;

            void setup();

        public:
            // Rule of five
            ~Program();
            Program(const Program& other) = delete;
            Program(Program&& other) = delete;
            Program& operator=(const Program& other) = delete;
            Program& operator=(Program&& other) = delete;
    };

    // class ProgramUser {
    //     public:
    //         // Placeholder
    // };

}
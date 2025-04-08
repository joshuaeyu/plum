#pragma once

#include <plum/context/asset.hpp>

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Core {

    class Program : public Asset::Asset {

        public:
            // May implement non-default UBO schemes in the future
            enum class UboScheme {
                Scheme1
            };

            Program(Path vertexShaderPath, Path fragmentShaderPath, Path geometryShaderPath = Path());
            // Program(const std::string& vertexShaderCode, const std::string& fragmentShaderCode, const std::string& geometryShaderCode = "");

            GLuint Handle() const { return handle; }

            void SetUniformBlockBindingScheme(UboScheme scheme = UboScheme::Scheme1);
            void SetUniformBlockBinding(const std::string& name, GLuint index);

            void SetInt(const std::string& name, int val);
            void SetFloat(const std::string& name, float val);
            void SetVec3(const std::string& name, const glm::vec3& vec);
            void SetMat4(const std::string& name, const glm::mat4& mat);

            void Use();
            
            // Asset::Asset
            void SyncWithDevice() override;
        
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
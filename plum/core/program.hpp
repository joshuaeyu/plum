#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Core {

    class Program {

        public:
            // May implement non-default UBO schemes in the future
            enum class UboScheme {
                Scheme1
            };

            GLuint handle;

            Program(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath = "");
            // Program(const std::string& vertexShaderCode, const std::string& fragmentShaderCode, const std::string& geometryShaderCode = "");
            
            // Rule of five
            ~Program();
            Program(const Program& other) = delete;
            Program(Program&& other) = delete;
            Program& operator=(const Program& other) = delete;
            Program& operator=(Program&& other) = delete;

            void SetUniformBlockBindingScheme(UboScheme scheme);
            void SetUniformBlockBinding(const std::string& name, GLuint index);

            void SetInt(const std::string& name, int val);
            void SetFloat(const std::string& name, float val);
            void SetVec3(const std::string& name, const glm::vec3& vec);
            void SetMat4(const std::string& name, const glm::mat4& mat);

            void Use();

    };

    // class ProgramUser {
    //     public:
    //         // Placeholder
    // };

}
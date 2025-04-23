#include <plum/core/program.hpp>
#include <plum/context/application.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <string>

namespace Core {

    Program::Program(std::shared_ptr<ShaderAsset> vert_shader, std::shared_ptr<ShaderAsset> frag_shader, std::shared_ptr<ShaderAsset> geom_shader)
        : vertexShader(vert_shader),
        fragmentShader(frag_shader),
        geometryShader(geom_shader)
    {
        setup();
    }

    Program::~Program()
    {
        glDeleteProgram(handle);
    }

    void Program::SetUniformBlockBindingScheme(UboScheme scheme) {
        switch (scheme) {
            case UboScheme::Scheme1:
                SetUniformBlockBinding("Matrices_Vertex", 0);
                SetUniformBlockBinding("Matrices_Fragment", 1);
                SetUniformBlockBinding("Camera", 2);
                SetUniformBlockBinding("DirLight", 3);
                SetUniformBlockBinding("PointLight", 4);
        }
    }

    void Program::SetUniformBlockBinding(const std::string& name, GLuint index) {
        GLuint ubo = glGetUniformBlockIndex(handle, name.c_str());
        glUniformBlockBinding(handle, ubo, index);
        // while (GLenum error = glGetError()) {
        //     std::cerr << "Warning: Uniform block "<< name << " was not found in the program. Ubo binding error: " << error << std::endl;
        // }
    }

    void Program::SetInt(const std::string& name, int val) {
        GLint location = glGetUniformLocation(handle, name.c_str());
        glUniform1i(location, val);
    }

    void Program::SetFloat(const std::string& name, float val) {
        GLint location = glGetUniformLocation(handle, name.c_str());
        glUniform1f(location, val);
    }

    void Program::SetVec3(const std::string& name, const glm::vec3& vec) {
        GLint location = glGetUniformLocation(handle, name.c_str());
        glUniform3fv(location, 1, glm::value_ptr(vec));
    }

    void Program::SetMat4(const std::string& name, const glm::mat4& mat) {
        GLint location = glGetUniformLocation(handle, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Program::Use() {
        glUseProgram(handle);
    }

    void Program::AssetResyncCallback() {
        glDeleteProgram(handle);
        setup();
    }

    void Program::setup() {
        handle = glCreateProgram();
        glAttachShader(handle, vertexShader->Handle());
        glAttachShader(handle, fragmentShader->Handle());
        if (geometryShader)
            glAttachShader(handle, geometryShader->Handle());

        // ==== Link shaders ====
        glLinkProgram(handle);
        GLint linkStatus;
        GLchar infoLog[1024];
        glGetProgramiv(handle, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus) {
            glGetProgramInfoLog(handle, 1024, NULL, infoLog);
            std::string errorMsg = "Failed to link program! ";
            errorMsg += infoLog;
            throw std::runtime_error(errorMsg);
        }

        SetUniformBlockBindingScheme(UboScheme::Scheme1);
        std::clog << "Successfully linked shaders: " << vertexShader->GetFile().Filename() << " " << fragmentShader->GetFile().Filename() << std::endl;
    }

}
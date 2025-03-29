#include <plum/core/program.hpp>
#include <plum/context/context.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

namespace Core {

    Program::Program(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath) 
        // : handle(glCreateProgram()) 
    {
        Context::Application::Instance();

        handle = glCreateProgram();

        std::cout << "  Loading shader: " << vertexShaderPath << " " << fragmentShaderPath << " " << geometryShaderPath << std::endl;

        // ==== File to cstring ====
        std::ifstream       vs_ifstream,        fs_ifstream,        gs_ifstream;
        std::stringstream   vs_stringstream,    fs_stringstream,    gs_stringstream;
        std::string         vs_string,          fs_string,          gs_string;
        const GLchar        *vs_cstr,           *fs_cstr,           *gs_cstr;
        
        vs_ifstream.open(vertexShaderPath); // same as calling constructor vs_ifstream(vertexShaderPath)
        fs_ifstream.open(fragmentShaderPath);
        if (vs_ifstream.fail())
            std::cerr << vertexShaderPath << " ERROR::SHADER::FILE_OPEN_FAIL" << std::endl;
        if (fs_ifstream.fail())
            std::cerr << fragmentShaderPath << " ERROR::SHADER::FILE_OPEN_FAIL" << std::endl;
        vs_stringstream << vs_ifstream.rdbuf();
        fs_stringstream << fs_ifstream.rdbuf();
        vs_string = vs_stringstream.str();
        fs_string = fs_stringstream.str();
        vs_cstr = vs_string.c_str();
        fs_cstr = fs_string.c_str();
        if (geometryShaderPath != "") {
            gs_ifstream.open(geometryShaderPath);
            if (gs_ifstream.fail())
                std::cerr << geometryShaderPath << "ERROR::SHADER::FILE_OPEN_FAIL" << std::endl;
            gs_stringstream << gs_ifstream.rdbuf();
            gs_string = gs_stringstream.str();
            gs_cstr = gs_string.c_str();
        }
        
        // ==== Compile shaders ====
        GLuint  vshader,          fshader,          gshader;
        GLint   vshader_compiled, fshader_compiled, gshader_compiled;
        GLchar  infoLog[1024];

        vshader = glCreateShader(GL_VERTEX_SHADER);
        fshader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vshader, 1, &vs_cstr, NULL);
        glShaderSource(fshader, 1, &fs_cstr, NULL);
        glAttachShader(handle, vshader);
        glAttachShader(handle, fshader);
        if (geometryShaderPath != "") {
            gshader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(gshader, 1, &gs_cstr, NULL);
            glAttachShader(handle, gshader);
        }
        
        glCompileShader(vshader);
        glGetShaderiv(vshader, GL_COMPILE_STATUS, &vshader_compiled);
        if (!vshader_compiled) {
            glGetShaderInfoLog(vshader, 1024, NULL, infoLog);
            std::cout << vertexShaderPath << " ERROR::SHADER::VERTEX::COMPILE_FAIL\n" << infoLog << std::endl;
            exit(-1);
        }
        glCompileShader(fshader);
        glGetShaderiv(fshader, GL_COMPILE_STATUS, &fshader_compiled);
        if (!fshader_compiled) {
            glGetShaderInfoLog(fshader, 1024, NULL, infoLog);
            std::cout << fragmentShaderPath << " ERROR::SHADER::FRAGMENT::COMPILE_FAIL\n" << infoLog << std::endl;
            exit(-1);
        }
        if (geometryShaderPath != "") {
            glCompileShader(gshader);
            glGetShaderiv(gshader, GL_COMPILE_STATUS, &gshader_compiled);
            if (!gshader_compiled) {
                glGetShaderInfoLog(gshader, 1024, NULL, infoLog);
                std::cout << geometryShaderPath << " ERROR::SHADER::GEOMETRY::COMPILE_FAIL\n" << infoLog << std::endl;
                exit(-1);
            }
        }

        // ==== Finally, link ====
        glLinkProgram(handle);
        GLint linkstatus;
        glGetProgramiv(handle, GL_LINK_STATUS, &linkstatus);
        if (!linkstatus) {
            glGetProgramInfoLog(handle, 1024, NULL, infoLog);
            std::cout << " ERROR::PROGRAM::LINKFAIL\n" << infoLog << std::endl;
            exit(-1);
        }
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
        while (GLenum error = glGetError()) {
            std::cerr << "Warning: Uniform block "<< name << " was not found in the program. Ubo binding error: " << error << std::endl;
        }
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

}
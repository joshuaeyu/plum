#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

using namespace std;

enum Shader_Enum {
    SHADER_UBO_SCHEME_1
};

class Shader {

    public:
        GLuint programID;

        Shader(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath = "") {
            std::cout << "  Loading shader: " << vertexShaderPath << " " << fragmentShaderPath << " " << geometryShaderPath << std::endl;
            
            programID = glCreateProgram();

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
            glAttachShader(programID, vshader);
            glAttachShader(programID, fshader);
            if (geometryShaderPath != "") {
                gshader = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(gshader, 1, &gs_cstr, NULL);
                glAttachShader(programID, gshader);
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
            glLinkProgram(programID);
        }

        void setUniformBlockBinding(Shader_Enum scheme) {
            switch (scheme) {
                case SHADER_UBO_SCHEME_1:
                    setUniformBlockBinding("Matrices_Vertex", 0);
                    setUniformBlockBinding("Matrices_Fragment", 1);
                    setUniformBlockBinding("Camera", 2);
                    setUniformBlockBinding("DirLight", 3);
                    setUniformBlockBinding("PointLight", 4);
                    break;
                default:
                    std::cerr << "Invalid Shader UBO scheme enum!\n";
                    exit(-1);
            }
        }

        void setUniformBlockBinding(std::string name, GLuint index) {
            GLuint ubo = glGetUniformBlockIndex(programID, name.c_str());
            glUniformBlockBinding(programID, ubo, index);
        }

        void setInt(std::string name, int val) {
            GLint location = glGetUniformLocation(programID, name.c_str());
            glUniform1i(location, val);
        }

        void setFloat(std::string name, float val) {
            GLint location = glGetUniformLocation(programID, name.c_str());
            glUniform1f(location, val);
        }
        
        void setVec3(std::string name, glm::vec3 vec) {
            GLint location = glGetUniformLocation(programID, name.c_str());
            glUniform3fv(location, 1, glm::value_ptr(vec));
        }

        void setMat4(std::string name, glm::mat4 mat) {
            GLint location = glGetUniformLocation(programID, name.c_str());
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        }

    private:

};

#endif
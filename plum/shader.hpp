#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

class Shader {

    public:

        enum Shader_Enum {
            SHADER_UBO_SCHEME_1
        };

        GLuint programID;

        Shader(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath = "");

        void setUniformBlockBinding(Shader_Enum scheme);

        void setUniformBlockBinding(std::string name, GLuint index);
        void setInt(std::string name, int val);
        void setFloat(std::string name, float val);
        void setVec3(std::string name, glm::vec3 vec);
        void setMat4(std::string name, glm::mat4 mat);

    private:

};

#endif
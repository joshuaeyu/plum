#include <plum/asset/shader.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdexcept>

ShaderAsset::ShaderAsset(const Path& path, GLenum shader_type) 
    : Asset(path),
    type(shader_type)
{}
    
ShaderAsset::~ShaderAsset() {
    glDeleteShader(handle);
}
    
GLuint ShaderAsset::Handle() {
    if (handle == -1) {
        setup();
    }
    return handle;
}

void ShaderAsset::SyncWithFile() {
    setup();
}

void ShaderAsset::setup() {
    handle = glCreateShader(type);
    std::string pathString = file.RawPath();

    std::cout << "Loading shader: " << file.Filename() << std::endl;

    // ==== File to cstring ====
    std::ifstream       ifs;
    std::stringstream   ss;
    std::string         s;
    const GLchar*       c;
    
    ifs.open(pathString); // same as calling constructor vs_ifstream(path)
    if (ifs.fail()) {
        std::string errorMsg = "Failed to open shader file ";
        errorMsg += file.RawPath();
        throw std::runtime_error(errorMsg);
    }
    ss << ifs.rdbuf();
    s = ss.str();
    c = s.c_str();

    // ==== Compile shader ====
    GLint   compileStatus;
    GLchar  infoLog[1024];

    glShaderSource(handle, 1, &c, NULL);
    
    glCompileShader(handle);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
        glGetShaderInfoLog(handle, 1024, NULL, infoLog);
        std::string errorMsg = "Failed to compile shader! ";
        errorMsg += infoLog;
        throw std::runtime_error(errorMsg);
    }
}
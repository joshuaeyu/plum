#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <map>
#include <string>

#include <plum/texture.hpp>
#include <plum/shape.hpp>

class Model;
class Shader;

class Resources {

    public:

        Resources();

        std::map<std::string, std::shared_ptr<Model>> Models;
        std::map<std::string, std::shared_ptr<Shape>> Shapes;
        std::map<std::string, std::shared_ptr<Tex>> Textures;
        std::map<std::string, std::shared_ptr<Shader>> Shaders;

        std::shared_ptr<Tex> LoadTexture2D(std::string name, std::string path, GLenum target, GLint wraptype, GLint filtertype = GL_LINEAR, Tex::Tex_Type textype = Tex::Tex_Type::TEX_DIFFUSE, bool flip = false);
        std::shared_ptr<Tex> LoadTextureCube(std::string name, std::vector<std::string>& faces_filenames, GLenum target, GLint wraptype, GLint filtertype = GL_LINEAR, bool flip = false);
        std::shared_ptr<Shader> LoadShader(std::string name, std::string vspath, std::string fspath, std::string gspath = "");

        std::shared_ptr<Model> LoadModel(std::string name, Shader& default_shader, std::string path, float scale = 1.0f, bool flip_textures = false);
        std::shared_ptr<Cube> LoadCube(std::string name, Shader& default_shader = *Shape::DefaultShaderColorOnly);
        std::shared_ptr<Sphere> LoadSphere(std::string name, Shader& default_shader = *Shape::DefaultShaderColorOnly, const unsigned int stack_count = 30, const unsigned int sector_count = 30);
        std::shared_ptr<Rectangle> LoadRectangle(std::string name, Shader& default_shader = *Shape::DefaultShaderColorOnly, const unsigned int x_sections = 1, const unsigned int z_sections = 1);

        void DeleteModel(std::string name);
        void DeleteShape(std::string name);
        void DeleteTexture(std::string name);
        // scene should recognize when drawing the same VAOs and instance them automatically

        // models: only differ in model transform
            // need to somehow store transforms for each model?
        // shapes:
        // use array texture for multi textures per draw call
        // use array of colors? for multi colors per draw call
        // pass index into array as an attribute

};

#endif
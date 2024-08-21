#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <map>
#include <string>

#include <plum/model.hpp>
#include <plum/shader.hpp>
#include <plum/shape.hpp>
#include <plum/texture.hpp>

using namespace std;

namespace Resources {

    map<string, shared_ptr<Model>> Models;
    map<string, shared_ptr<Shape>> Shapes;
    map<string, shared_ptr<Tex>> Textures;
    map<string, shared_ptr<Shader>> Shaders;

    shared_ptr<Tex> LoadTexture2D(string name, string path, GLenum target, GLint wraptype, GLint filtertype = GL_LINEAR, Tex_Type textype = TEX_DIFFUSE, bool flip = false) {
        shared_ptr<Tex> t(new Tex(name, path, target, wraptype, filtertype, textype, flip));
        Textures.insert(pair<string, shared_ptr<Tex>>(name, t));
        return t;
    }
    shared_ptr<Tex> LoadTextureCube(string name, std::vector<std::string>& faces_filenames, GLenum target, GLint wraptype, GLint filtertype = GL_LINEAR, bool flip = false) {
        shared_ptr<Tex> t(new Tex(name, faces_filenames, target, wraptype, filtertype, flip));
        Textures.insert(pair<string, shared_ptr<Tex>>(name, t));
        return t;
    }
    shared_ptr<Shader> LoadShader(string name, string vspath, string fspath, string gspath = "") {
        shared_ptr<Shader> s(new Shader(vspath, fspath, gspath));
        Shaders.insert(pair<string, shared_ptr<Shader>>(name, s));
        return s;
    }

    shared_ptr<Model> LoadModel(string name, Shader& default_shader, string path, float scale = 1.0f, bool flip_textures = false) {
        shared_ptr<Model> m(new Model(name, path, &default_shader, scale, flip_textures));
        Models.insert(pair<string, shared_ptr<Model>>(name, m));
        return m;
    }
    shared_ptr<Cube> LoadCube(string name, Shader& default_shader) {
        shared_ptr<Cube> c(new Cube(name, &default_shader));
        Shapes.insert(pair<string, shared_ptr<Shape>>(name, c));
        return c;
    }
    shared_ptr<Sphere> LoadSphere(string name, Shader& default_shader, const unsigned int stack_count = 30, const unsigned int sector_count = 30) {
        shared_ptr<Sphere> s(new Sphere(name, &default_shader, stack_count, sector_count));
        Shapes.insert(pair<string, shared_ptr<Shape>>(name, s));
        return s;
    }
    shared_ptr<Rectangle> LoadRectangle(string name, Shader& default_shader, const unsigned int x_sections = 1, const unsigned int z_sections = 1) {
        shared_ptr<Rectangle> r(new Rectangle(name, &default_shader, x_sections, z_sections));
        Shapes.insert(pair<string, shared_ptr<Shape>>(name, r));
        return r;
    }

    void DeleteModel(string name) {
        Models.erase(name);
    }
    void DeleteShape(string name) {
        Shapes.erase(name);
    }
    void DeleteTexture(string name) {
        Textures.erase(name);
    }
    // scene should recognize when drawing the same VAOs and instance them automatically

    // models: only differ in model transform
        // need to somehow store transforms for each model?
    // shapes:
    // use array texture for multi textures per draw call
    // use array of colors? for multi colors per draw call
    // pass index into array as an attribute

}

#endif
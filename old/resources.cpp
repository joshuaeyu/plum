#include <plum/resources.hpp>

#include <map>
#include <string>

#include <plum/model.hpp>
#include <plum/shader.hpp>
#include <plum/shape.hpp>
#include <plum/texture.hpp>

Resources::Resources() {}

std::shared_ptr<Tex> Resources::LoadTexture2D(std::string name, std::string path, GLenum target, GLint wraptype, GLint filtertype, Tex::Tex_Type textype, bool flip) {
    std::shared_ptr<Tex> t(new Tex(name, path, target, wraptype, filtertype, textype, flip));
    Textures.insert(std::pair<std::string, std::shared_ptr<Tex>>(name, t));
    return t;
}
std::shared_ptr<Tex> Resources::LoadTextureCube(std::string name, std::vector<std::string>& faces_filenames, GLenum target, GLint wraptype, GLint filtertype, bool flip) {
    std::shared_ptr<Tex> t(new Tex(name, faces_filenames, target, wraptype, filtertype, flip));
    Textures.insert(std::pair<std::string, std::shared_ptr<Tex>>(name, t));
    return t;
}
std::shared_ptr<Shader> Resources::LoadShader(std::string name, std::string vspath, std::string fspath, std::string gspath) {
    std::shared_ptr<Shader> s(new Shader(vspath, fspath, gspath));
    Shaders.insert(std::pair<std::string, std::shared_ptr<Shader>>(name, s));
    return s;
}

std::shared_ptr<Model> Resources::LoadModel(std::string name, Shader& default_shader, std::string path, float scale, bool flip_textures) {
    std::shared_ptr<Model> m(new Model(name, path, &default_shader, scale, flip_textures));
    Models.insert(std::pair<std::string, std::shared_ptr<Model>>(name, m));
    return m;
}
std::shared_ptr<Cube> Resources::LoadCube(std::string name, Shader& default_shader) {
    std::shared_ptr<Cube> c(new Cube(name, &default_shader));
    Shapes.insert(std::pair<std::string, std::shared_ptr<Shape>>(name, c));
    return c;
}
std::shared_ptr<Sphere> Resources::LoadSphere(std::string name, Shader& default_shader, const unsigned int stack_count, const unsigned int sector_count) {
    std::shared_ptr<Sphere> s(new Sphere(name, &default_shader, stack_count, sector_count));
    Shapes.insert(std::pair<std::string, std::shared_ptr<Shape>>(name, s));
    return s;
}
std::shared_ptr<Rectangle> Resources::LoadRectangle(std::string name, Shader& default_shader, const unsigned int x_sections, const unsigned int z_sections) {
    std::shared_ptr<Rectangle> r(new Rectangle(name, &default_shader, x_sections, z_sections));
    Shapes.insert(std::pair<std::string, std::shared_ptr<Shape>>(name, r));
    return r;
}

void Resources::DeleteModel(std::string name) {
    Models.erase(name);
}
void Resources::DeleteShape(std::string name) {
    Shapes.erase(name);
}
void Resources::DeleteTexture(std::string name) {
    Textures.erase(name);
}
// scene should recognize when drawing the same VAOs and instance them automatically

// models: only differ in model transform
    // need to somehow store transforms for each model?
// shapes:
// use array texture for multi textures per draw call
// use array of colors? for multi colors per draw call
// pass index into array as an attribute

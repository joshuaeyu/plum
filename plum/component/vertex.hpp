#pragma once

#include <glm/glm.hpp>
#include <map>

namespace Component {

    enum VertexAttrFlags : uint8_t {
        Position  = 0b0000001,
        Normal    = 0b0000010,
        Uv        = 0b0000100,
        Tangent   = 0b0001000,
        Bitangent = 0b0010000,
        All       = 0b0011111
    };
    inline VertexAttrFlags operator|(const VertexAttrFlags& f1, const VertexAttrFlags& f2) {
        return static_cast<VertexAttrFlags>(static_cast<uint8_t>(f1) | static_cast<uint8_t>(f2));
    }
    inline VertexAttrFlags operator&(const VertexAttrFlags& f1, const VertexAttrFlags& f2) {
        return static_cast<VertexAttrFlags>(static_cast<uint8_t>(f1) & static_cast<uint8_t>(f2));
    }
    inline VertexAttrFlags operator*(const int x, const VertexAttrFlags& f) {
        return static_cast<VertexAttrFlags>((x) * static_cast<uint8_t>(f));
    }
    
    struct VertexAttrType {
        VertexAttrFlags flag;
        int numcomps;
        size_t size;
    };
    static const std::vector<VertexAttrType> VertexAttrTypes = {
        {Position,  3, sizeof(glm::vec3)},
        {Normal,    3, sizeof(glm::vec3)},
        {Uv,        2, sizeof(glm::vec2)},
        {Tangent,   3, sizeof(glm::vec3)},
        {Bitangent, 3, sizeof(glm::vec3)}
    };

    class UncollatedVertices {
        public:
            UncollatedVertices();
            
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> uvs;
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec3> bitangents;
    };

    class CollatedVertices {
        // want bit flags but also want to abstract iteration over the bits
        public:
            CollatedVertices(const UncollatedVertices& uncollated);
            CollatedVertices(const std::vector<float>& uncollated, VertexAttrFlags attrflags = VertexAttrFlags::All);

            bool HasProperty(const VertexAttrFlags attrflag) const; //  { return (attributes & attrflag) != 0; }
            size_t PropertyOffset(const VertexAttrFlags attrflag) const;

            size_t StrideLength() const;

            std::vector<float> data;
        private:
            VertexAttrFlags attributes;
            size_t stride;
    };

    class Vertex {
        public:
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
            glm::vec3 tangent;
            glm::vec3 bitangent;
            
            Vertex() {}
            
        protected:
        private:
    };

}
#pragma once

#include <plum/core/attribute.hpp>

#include <vector>

namespace Core {

    namespace Vertex {

        class VertexArray {
            public:
                struct UncollatedVertices {
                    std::vector<float> positions, normals, uvs, tangents, bitangents;
                };
                /* Construct using a collated vector of attribute data. Except for Position, all attributes are optional. Attributes provided must be in the following order:
                    - Position2/Position3
                    - Normal
                    - Uv
                    - Tangent
                    - Bitangent */
                VertexArray(const std::vector<float>& collated, AttrFlags flags = AttrFlags::Default3D);
                /* Construct using an instance the UncollatedVertices struct. */
                VertexArray(const UncollatedVertices& uncollated);

            public:
                std::vector<float> AttributeData(AttrFlags flag) const;
                bool HasAttributes(AttrFlags flags) const;
                size_t AttributeOffset(AttrFlags flag) const;
                
                int VertexCount() const;
                size_t Size() const;
                size_t Stride() const;
                
                const std::vector<float>& Data() const;
                
            private:
                std::vector<float> data;
                AttrFlags attributes;
                size_t stride;
                int vertexCount;
        };

    }

}
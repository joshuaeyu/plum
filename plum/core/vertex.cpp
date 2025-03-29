#include <plum/core/vertex.hpp>

namespace Core {
    
    namespace Vertex {

        VertexArray::VertexArray(const std::vector<float>& collated, unsigned int flags) 
            : data(collated), 
            attributes(flags) 
        {
            // Compute stride
            stride = 0;
            for (const auto& attr : AttrTypes) {
                if (HasAttributes(attr.flag))
                    stride += attr.size;
            }
            // Determine vertex count
            vertexCount = data.size() * sizeof(float) / Stride();
        }

        VertexArray::VertexArray(const UncollatedVertices& uncollated) 
        {
            // Determine attribute flags
            attributes = 0;
            attributes = attributes | ((uncollated.positions.size() > 0) * AttrFlags::Position3); // Assume three dimensional position vector if provided this way
            attributes = attributes | ((uncollated.normals.size() > 0) * AttrFlags::Normal);
            attributes = attributes | ((uncollated.uvs.size() > 0) * AttrFlags::Uv);
            attributes = attributes | ((uncollated.tangents.size() > 0) * AttrFlags::Tangent);
            attributes = attributes | ((uncollated.bitangents.size() > 0) * AttrFlags::Bitangent);

            // Determine vertex count
            vertexCount = 0;
            if (HasAttributes(AttrFlags::Position3)) {
                vertexCount = uncollated.positions.size() / AttrPosition3.ncomps;
            } else if (HasAttributes(AttrFlags::Normal)) {
                vertexCount = uncollated.normals.size() / AttrNormal.ncomps;
            } else if (HasAttributes(AttrFlags::Uv)) {
                vertexCount = uncollated.uvs.size() / AttrUv.ncomps;
            } else if (HasAttributes(AttrFlags::Tangent)) {
                vertexCount = uncollated.tangents.size() / AttrTangent.ncomps;
            } else if (HasAttributes(AttrFlags::Bitangent)) {
                vertexCount = uncollated.bitangents.size() / AttrBitangent.ncomps;
            }
            
            // Assembled collated data vector
            for (int i = 0; i < vertexCount; i++) {
                if (HasAttributes(AttrFlags::Position3)) {
                    int ndims = AttrPosition3.ncomps;
                    for (int j = ndims * i; j < ndims * (i + 1); j++)
                        data.push_back(uncollated.positions[j]);
                }
                if (HasAttributes(AttrFlags::Normal)) {
                    int ndims = AttrNormal.ncomps;
                    for (int j = ndims * i; j < ndims * (i + 1); j++)
                        data.push_back(uncollated.normals[j]);
                }
                if (HasAttributes(AttrFlags::Uv)) {
                    int ndims = AttrUv.ncomps;
                    for (int j = ndims * i; j < ndims * (i + 1); j++)
                        data.push_back(uncollated.uvs[j]);
                }
                if (HasAttributes(AttrFlags::Tangent)) {
                    int ndims = AttrTangent.ncomps;
                    for (int j = ndims * i; j < ndims * (i + 1); j++)
                        data.push_back(uncollated.tangents[j]);
                }
                if (HasAttributes(AttrFlags::Bitangent)) {
                    int ndims = AttrBitangent.ncomps;
                    for (int j = ndims * i; j < ndims * (i + 1); j++)
                        data.push_back(uncollated.bitangents[j]);
                }
            }

            // Compute stride
            stride = 0;
            for (const auto& attr : AttrTypes) {
                if (HasAttributes(attr.flag))
                    stride += attr.size;
            }
        }

        std::vector<float> VertexArray::AttributeData(AttrFlags flag) const {
            std::vector<float> result;
            int startIdx = AttributeOffset(flag) / sizeof(float);
            int strideLength = Stride() / sizeof(float);
            auto attr = GetAttrTypeInfo(flag);
            for (int i = startIdx; i < data.size(); i += strideLength) {
                for (int j = i; j < i + attr.ncomps; j++)
                    result.push_back(data[j]);
            }
            return result;
        }

        bool VertexArray::HasAttributes(AttrFlags flags) const {
            return !(~attributes & flags);
        }

        size_t VertexArray::AttributeOffset(AttrFlags flag) const {
            size_t offset = 0;
            for (const auto& attr : AttrTypes) {
                if (HasAttributes(attr.flag)) {
                    if (attr.index == GetAttrTypeInfo(flag).index)
                        break;
                    else
                        offset += attr.size;
                }
            }
            return offset;
        }
        
        int VertexArray::VertexCount() const {
            return vertexCount;
        }

        size_t VertexArray::Size() const {
            return data.size() * sizeof(float);
        }

        size_t VertexArray::Stride() const {
            return stride;
        }
        
        const std::vector<float>& VertexArray::Data() const {
            return data;
        }
    }
}
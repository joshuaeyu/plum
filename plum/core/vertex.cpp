#include <plum/core/vertex.hpp>

namespace Core {
    
    VertexArray::VertexArray(const std::vector<float>& collated, const unsigned int flags) 
        : data(collated), 
        attributes(flags) 
    {
        // Compute stride
        stride = 0;
        for (auto attr : VertexAttrInfo.AttrList) {
            if (HasAttributes(attr.flag))
                stride += attr.size;
        }
        // Determine vertex count
        vertexCount = Size() / Stride();
    }

    VertexArray::VertexArray(const UncollatedVertices& uncollated) 
    {
        // Determine attribute flags
        attributes = 0;
        attributes = attributes | ((uncollated.positions.size() > 0) * VertexAttrFlags::Position3); // Assume three dimensional position vector if provided this way
        attributes = attributes | ((uncollated.normals.size() > 0) * VertexAttrFlags::Normal);
        attributes = attributes | ((uncollated.uvs.size() > 0) * VertexAttrFlags::Uv);
        attributes = attributes | ((uncollated.tangents.size() > 0) * VertexAttrFlags::Tangent);
        attributes = attributes | ((uncollated.bitangents.size() > 0) * VertexAttrFlags::Bitangent);

        // Determine vertex count
        vertexCount = 0;
        if (HasAttributes(VertexAttrFlags::Position3)) {
            vertexCount = uncollated.positions.size() / VertexAttrInfo.Position3.ncomps;
        } else if (HasAttributes(VertexAttrFlags::Normal)) {
            vertexCount = uncollated.normals.size() / VertexAttrInfo.Normal.ncomps;
        } else if (HasAttributes(VertexAttrFlags::Uv)) {
            vertexCount = uncollated.uvs.size() / VertexAttrInfo.Uv.ncomps;
        } else if (HasAttributes(VertexAttrFlags::Tangent)) {
            vertexCount = uncollated.tangents.size() / VertexAttrInfo.Tangent.ncomps;
        } else if (HasAttributes(VertexAttrFlags::Bitangent)) {
            vertexCount = uncollated.bitangents.size() / VertexAttrInfo.Bitangent.ncomps;
        }
        
        // Assembled collated data vector
        for (int i = 0; i < vertexCount; i++) {
            if (HasAttributes(VertexAttrFlags::Position3)) {
                int ndims = VertexAttrInfo.Position3.ncomps;
                for (int j = ndims * i; j < ndims * (i + 1); j++)
                    data.push_back(uncollated.positions[j]);
            }
            if (HasAttributes(VertexAttrFlags::Normal)) {
                int ndims = VertexAttrInfo.Normal.ncomps;
                for (int j = ndims * i; j < ndims * (i + 1); j++)
                    data.push_back(uncollated.normals[j]);
            }
            if (HasAttributes(VertexAttrFlags::Uv)) {
                int ndims = VertexAttrInfo.Uv.ncomps;
                for (int j = ndims * i; j < ndims * (i + 1); j++)
                    data.push_back(uncollated.uvs[j]);
            }
            if (HasAttributes(VertexAttrFlags::Tangent)) {
                int ndims = VertexAttrInfo.Tangent.ncomps;
                for (int j = ndims * i; j < ndims * (i + 1); j++)
                    data.push_back(uncollated.tangents[j]);
            }
            if (HasAttributes(VertexAttrFlags::Bitangent)) {
                int ndims = VertexAttrInfo.Bitangent.ncomps;
                for (int j = ndims * i; j < ndims * (i + 1); j++)
                    data.push_back(uncollated.bitangents[j]);
            }
        }

        // Compute stride
        stride = 0;
        for (auto attr : VertexAttrInfo.AttrList) {
            if (HasAttributes(attr.flag))
                stride += attr.size;
        }
    }

    std::vector<float> VertexArray::AttributeData(const VertexAttrFlags flag) const {
        std::vector<float> result;
        int startIdx = AttributeOffset(flag) / sizeof(float);
        int strideLength = Stride() / sizeof(float);
        auto attr = VertexAttrInfo::GetInfo(flag);
        for (int i = startIdx; i < data.size(); i += strideLength) {
            for (int j = i; j < i + attr.ncomps; j++)
                result.push_back(data[j]);
        }
        return result;
    }

    bool VertexArray::HasAttributes(const VertexAttrFlags flags) const {
        return !(~attributes & flags);
    }

    size_t VertexArray::AttributeOffset(const VertexAttrFlags flag) const {
        size_t offset = 0;
        for (auto attr : VertexAttrInfo.AttrList) {
            if (attr.flag == flag)
                break;
            else
                offset += attr.size;
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
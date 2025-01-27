#include <plum/component/vertex.hpp>

namespace Component {

    UncollatedVertices::UncollatedVertices() {}
    
    CollatedVertices::CollatedVertices(const UncollatedVertices &uncollated) {
        // Determine attribute flags
        attributes = attributes | ((uncollated.positions.size() > 0) * VertexAttrFlags::Position);
        attributes = attributes | ((uncollated.normals.size() > 0) * VertexAttrFlags::Normal);
        attributes = attributes | ((uncollated.uvs.size() > 0) * VertexAttrFlags::Uv);
        attributes = attributes | ((uncollated.tangents.size() > 0) * VertexAttrFlags::Tangent);
        attributes = attributes | ((uncollated.bitangents.size() > 0) * VertexAttrFlags::Bitangent);

        // Assembled collated data vector
        int count = uncollated.positions.size();
        for (int i = 0; i < count; i++) {
            if (attributes & VertexAttrFlags::Position) {
                data.push_back(uncollated.positions[i].x);
                data.push_back(uncollated.positions[i].y);
                data.push_back(uncollated.positions[i].z);
            }
            if (attributes & VertexAttrFlags::Normal) {
                data.push_back(uncollated.normals[i].x);
                data.push_back(uncollated.normals[i].y);
                data.push_back(uncollated.normals[i].z);
            }
            if (attributes & VertexAttrFlags::Uv) {
                data.push_back(uncollated.uvs[i].x);
                data.push_back(uncollated.uvs[i].y);
            }
            if (attributes & VertexAttrFlags::Tangent) {
                data.push_back(uncollated.tangents[i].x);
                data.push_back(uncollated.tangents[i].y);
                data.push_back(uncollated.tangents[i].z);
            }
            if (attributes & VertexAttrFlags::Bitangent) {
                data.push_back(uncollated.bitangents[i].x);
                data.push_back(uncollated.bitangents[i].y);
                data.push_back(uncollated.bitangents[i].z);
            }
        }

        // calc stride
    }

    CollatedVertices::CollatedVertices(const std::vector<float>& collated, VertexAttrFlags attrflags) : data(collated), attributes(attrflags) {
        // calc stride
    }

    bool CollatedVertices::HasProperty(const VertexAttrFlags attrflag) const { 
        return (attributes & attrflag) != 0; 
    }
    size_t CollatedVertices::PropertyOffset(const VertexAttrFlags attrflag) const {
        // calc offset
    }

    size_t CollatedVertices::StrideLength() const {
        return stride;
    }
}
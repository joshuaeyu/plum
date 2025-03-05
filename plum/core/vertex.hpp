#pragma once

#include <vector>

namespace Core {

    enum VertexAttrFlags : unsigned int {
        None       =      0,
        Position2  = 1 << 0,
        Position3  = 1 << 1,
        Normal     = 1 << 2,
        Uv         = 1 << 3,
        Tangent    = 1 << 4,
        Bitangent  = 1 << 5,
        All        =    -1U
    };
    // inline VertexAttrFlags operator|(const VertexAttrFlags& f1, const VertexAttrFlags& f2) {
    //     return static_cast<VertexAttrFlags>(static_cast<uint8_t>(f1) | static_cast<uint8_t>(f2));
    // }
    // inline VertexAttrFlags operator&(const VertexAttrFlags& f1, const VertexAttrFlags& f2) {
    //     return static_cast<VertexAttrFlags>(static_cast<uint8_t>(f1) & static_cast<uint8_t>(f2));
    // }
    // inline VertexAttrFlags operator*(const int x, const VertexAttrFlags& f) {
    //     return static_cast<VertexAttrFlags>((x) * static_cast<uint8_t>(f));
    // }
    
    static struct VertexAttrInfo {
        struct VertexAttrTypeInfo {
            VertexAttrTypeInfo(const VertexAttrFlags flag, const int ncomps) : flag(flag), ncomps(ncomps), size(sizeof(float)*ncomps) {}
            const VertexAttrFlags flag;
            const int ncomps;
            const size_t size;
        };
        
        inline static const VertexAttrTypeInfo Position2 = {VertexAttrFlags::Position2, 2};
        inline static const VertexAttrTypeInfo Position3 = {VertexAttrFlags::Position3, 3};
        inline static const VertexAttrTypeInfo Normal = {VertexAttrFlags::Normal, 3};
        inline static const VertexAttrTypeInfo Uv = {VertexAttrFlags::Uv, 2};
        inline static const VertexAttrTypeInfo Tangent = {VertexAttrFlags::Tangent, 3};
        inline static const VertexAttrTypeInfo Bitangent = {VertexAttrFlags::Bitangent, 3};
        
        inline static const std::vector<VertexAttrTypeInfo> AttrList = {Position2, Position3, Normal, Uv, Tangent, Bitangent};

        static const VertexAttrTypeInfo GetInfo(const VertexAttrFlags flag) {
            for (auto attr : AttrList) {
                if (attr.flag == flag) {
                    return attr;
                }
            }
            return VertexAttrTypeInfo{VertexAttrFlags::None, 0};
        }
    } VertexAttrInfo;

    class VertexArray {
        public:
            struct UncollatedVertices {
                std::vector<float> positions;
                std::vector<float> normals;
                std::vector<float> uvs;
                std::vector<float> tangents;
                std::vector<float> bitangents;
            };

            VertexArray(const std::vector<float>& collated, const unsigned int flags = VertexAttrFlags::All);
            VertexArray(const UncollatedVertices& uncollated);

        public:
            std::vector<float> AttributeData(const VertexAttrFlags flag) const;
            bool HasAttributes(const VertexAttrFlags flags) const;
            size_t AttributeOffset(const VertexAttrFlags flag) const;
            
            int VertexCount() const;
            size_t Size() const;
            size_t Stride() const;
            
            const std::vector<float>& Data() const;
            
        private:
            std::vector<float> data;
            unsigned int attributes;
            size_t stride;
            int vertexCount;
    };

}
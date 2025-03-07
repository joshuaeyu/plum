#pragma once

#include <vector>

namespace Core {

    namespace Vertex {

        enum AttrFlags : unsigned int {
            None       =      0,
            Position3  = 1 << 0,
            Normal     = 1 << 1,
            Uv         = 1 << 2,
            Tangent    = 1 << 3,
            Bitangent  = 1 << 4,
            Default3D  = (1 << 5) - 1,
            
            Position2  = 1 << 5
        };
        // inline AttrFlags operator|(const AttrFlags& f1, const AttrFlags& f2) {
        //     return static_cast<AttrFlags>(static_cast<uint8_t>(f1) | static_cast<uint8_t>(f2));
        // }
        // inline AttrFlags operator&(const AttrFlags& f1, const AttrFlags& f2) {
        //     return static_cast<AttrFlags>(static_cast<uint8_t>(f1) & static_cast<uint8_t>(f2));
        // }
        // inline AttrFlags operator*(const int x, const AttrFlags& f) {
        //     return static_cast<AttrFlags>((x) * static_cast<uint8_t>(f));
        // }
        
        struct AttrTypeInfo {
            AttrTypeInfo(const AttrFlags flag, const int index, const int ncomps) 
                : flag(flag), 
                index(index), 
                ncomps(ncomps), 
                size(sizeof(float)*ncomps) 
            {}
            const AttrFlags flag;
            const int index;
            const int ncomps;
            const size_t size;
        };
        
        inline static const AttrTypeInfo AttrPosition2{AttrFlags::Position2, 0, 2};
        inline static const AttrTypeInfo AttrPosition3{AttrFlags::Position3, 0, 3};
        inline static const AttrTypeInfo AttrNormal   {AttrFlags::Normal,    1, 3};
        inline static const AttrTypeInfo AttrUv       {AttrFlags::Uv,        2, 2};
        inline static const AttrTypeInfo AttrTangent  {AttrFlags::Tangent,   3, 3};
        inline static const AttrTypeInfo AttrBitangent{AttrFlags::Bitangent, 4, 3};
        
        inline static const std::vector<AttrTypeInfo> AttrTypes = {AttrPosition2, AttrPosition3, AttrNormal, AttrUv, AttrTangent, AttrBitangent};

        inline static const AttrTypeInfo GetAttrTypeInfo(const AttrFlags flag) {
            for (const auto& attr : AttrTypes) {
                if (attr.flag == flag) {
                    return attr;
                }
            }
            return AttrTypeInfo{AttrFlags::None, -1, 0};
        }

        class VertexArray {
            public:
                struct UncollatedVertices {
                    std::vector<float> positions;
                    std::vector<float> normals;
                    std::vector<float> uvs;
                    std::vector<float> tangents;
                    std::vector<float> bitangents;
                };

                VertexArray(const std::vector<float>& collated, const unsigned int flags = AttrFlags::Default3D);
                VertexArray(const UncollatedVertices& uncollated);

            public:
                std::vector<float> AttributeData(const AttrFlags flag) const;
                bool HasAttributes(const AttrFlags flags) const;
                size_t AttributeOffset(const AttrFlags flag) const;
                
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

}
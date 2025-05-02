#pragma once

#include <utility>
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

        constexpr AttrFlags operator|(const AttrFlags& a, const AttrFlags& b) {
            return static_cast<AttrFlags>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
        }
        constexpr AttrFlags& operator|=(AttrFlags& a, const AttrFlags& b) {
            a = std::move(a | b);
            return a;
        }
        constexpr AttrFlags operator&(const AttrFlags& a, const AttrFlags& b) {
            return static_cast<AttrFlags>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
        }
        constexpr AttrFlags& operator&=(AttrFlags& a, const AttrFlags& b) {
            a = std::move(a & b);
            return a;
        }
        constexpr AttrFlags operator^(const AttrFlags& a, const AttrFlags& b) {
            return static_cast<AttrFlags>(static_cast<unsigned int>(a) ^ static_cast<unsigned int>(b));
        }
        constexpr AttrFlags& operator^=(AttrFlags& a, const AttrFlags& b) {
            a = std::move(a ^ b);
            return a;
        }
        constexpr AttrFlags operator*(const int x, const AttrFlags& f) {
            return static_cast<AttrFlags>((x) * static_cast<unsigned int>(f));
        }
        constexpr AttrFlags operator&(const AttrFlags& f) {
            return static_cast<AttrFlags>(~static_cast<unsigned int>(f));
        }

        struct Attribute {
            Attribute(AttrFlags flag, int index, int ncomps) 
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
        
        inline static const Attribute AttrPosition2{AttrFlags::Position2, 0, 2};
        inline static const Attribute AttrPosition3{AttrFlags::Position3, 0, 3};
        inline static const Attribute AttrNormal   {AttrFlags::Normal,    1, 3};
        inline static const Attribute AttrUv       {AttrFlags::Uv,        2, 2};
        inline static const Attribute AttrTangent  {AttrFlags::Tangent,   3, 3};
        inline static const Attribute AttrBitangent{AttrFlags::Bitangent, 4, 3};
        
        inline static const std::vector<Attribute> Attributes = {AttrPosition2, AttrPosition3, AttrNormal, AttrUv, AttrTangent, AttrBitangent};
        
        inline static const Attribute AttributeFromFlag(AttrFlags flag) {
            for (const auto& attr : Attributes) {
                if (attr.flag == flag) {
                    return attr;
                }
            }
            static const Attribute None{AttrFlags::None, -1, 0};
            return None;
        }
        
    }

}


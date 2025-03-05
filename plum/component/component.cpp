#include <plum/component/component.hpp>

namespace Component {

    Component::Component(const ComponentType type) 
        : type(type) 
    {}
    Component::~Component() {}

    const bool Component::IsMesh() const {
        return type == ComponentType::Mesh
                || type == ComponentType::Model
                || type == ComponentType::Primitive;
    }
    const bool Component::IsLight() const {
        return type == ComponentType::DirLight
                || type == ComponentType::PointLight;
    }

}
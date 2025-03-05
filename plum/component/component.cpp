#include <plum/component/component.hpp>

namespace Component {

    ComponentBase::ComponentBase(const ComponentType type) 
        : type(type) 
    {}
    ComponentBase::~ComponentBase() {}

    const bool ComponentBase::IsMesh() const {
        return type == ComponentType::Mesh
                || type == ComponentType::Model
                || type == ComponentType::Primitive;
    }
    const bool ComponentBase::IsLight() const {
        return type == ComponentType::DirLight
                || type == ComponentType::PointLight;
    }

}
#pragma once

#include <glm/glm.hpp>

namespace Direction {
    constexpr glm::vec3 right   = glm::vec3( 1,  0,  0);
    constexpr glm::vec3 left    = glm::vec3(-1,  0,  0);
    constexpr glm::vec3 up      = glm::vec3( 0,  1,  0);
    constexpr glm::vec3 down    = glm::vec3( 0, -1,  0);
    constexpr glm::vec3 front   = glm::vec3( 0,  0,  1);
    constexpr glm::vec3 back    = glm::vec3( 0,  0, -1);
}
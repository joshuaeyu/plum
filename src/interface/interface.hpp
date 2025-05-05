#pragma once

#include "context/window.hpp"

#include <imgui/imgui.h>

namespace Interface {

    void Initialize(Context::Window& window);
    void BeginFrame();
    void RenderFrame();

    void setStyle(ImGuiStyle& style);

};
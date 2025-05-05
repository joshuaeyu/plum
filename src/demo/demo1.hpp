#pragma once

#include "demo/demo.hpp"

#include "component/all.hpp"
#include "context/all.hpp"
#include "core/all.hpp"
#include "renderer/all.hpp"
#include "scene/all.hpp"

#include <set>

class Demo1 : public Demo {
    public:
        Demo1();
        ~Demo1() = default;

    protected:
        void initialize() override;
        void displayScene() override;
        void displayGui() override;
        void cleanUp() override;
};
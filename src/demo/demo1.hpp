#pragma once

#include "demo/demo.hpp"

class Demo1 : public Demo {
    public:
        Demo1();
        ~Demo1() = default;

    protected:
        void initialize() override;
        void preDisplayScene() override;
        void postDisplayScene() override;
        void displayGui() override;
        void cleanUp() override;
};
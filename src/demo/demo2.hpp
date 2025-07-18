#pragma once

#include "demo/demo.hpp"

class Demo2 : public Demo {
    public:
        Demo2();
        ~Demo2() = default;

    protected:
        void initialize() override;
        void preDisplayScene() override;
        void postDisplayScene() override;
        void displayGui() override;
        void cleanUp() override;
};
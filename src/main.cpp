#include "context/application.hpp"
#include "demo/all.hpp"

#include <iostream>

int main() {
    std::clog << "Initializing application..." << std::endl;
    Context::Application& app = Context::Application::Instance();
    auto demo1 = std::make_shared<Demo1>();
    auto demo2 = std::make_shared<Demo2>();
    app.demos = {demo1, demo2};
    app.activeDemo = demo1;

    std::clog << "Starting application..." << std::endl;
    app.Run();
}
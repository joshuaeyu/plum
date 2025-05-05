#include "context/all.hpp"
#include "demo/demo1.hpp"

#include <iostream>

int main() {
    std::clog << "Initializing application..." << std::endl;
    Context::Application& app = Context::Application::Instance();
    auto demo1 = std::make_shared<Demo1>();
    app.demos = {demo1};

    std::clog << "Starting application..." << std::endl;
    app.Run();
}
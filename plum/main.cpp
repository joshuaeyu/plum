#include <plum/context/all.hpp>
#include <plum/demo/demo1.hpp>

#include <iostream>

int main() {
    std::clog << "Initializing application..." << std::endl;
    Context::Application& app = Context::Application::Instance();
    app.activeDemo = std::make_shared<Demo1>();

    std::clog << "Starting application..." << std::endl;
    app.Run();
}
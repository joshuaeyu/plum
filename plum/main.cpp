#include <plum/context/all.hpp>
#include <plum/demo/demo1.hpp>

#include <iostream>

int main() {
    std::cout << "Initializing application..." << std::endl;
    Context::Application& app = Context::Application::Instance();
    app.activeDemo = std::make_shared<Demo1>();

    std::cout << "Starting application..." << std::endl;
    app.Run();
}
#include "Application.hpp"

#include <memory>

int main() {

    auto app = std::make_unique<Application>();
    app->Init();
    app->Run();
    app->Quit();

    return 0;
}

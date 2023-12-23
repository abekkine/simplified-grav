#include "Display.hpp"

#include "GravSim.hpp"

#include <memory>

std::shared_ptr<GravSim> sim;

void init_display() {
    DISPLAY.SetWindowSize(1280, 800);
    DISPLAY.SetWindowPosition(50, 50);
    DISPLAY.SetWindowTitle("gravsim");
    DISPLAY.SetCenter(0.0, 0.0);
    DISPLAY.SetPixelSize(30000000.0 / 800.0);
    DISPLAY.SetPanAndZoom(true);
    DISPLAY.Init();
}

void render_world() {
    sim->Step(10.0);
    sim->RenderWorld();
}

void render_ui() {}

void run_loop() {
    while (!DISPLAY.QuitCondition()) {
        DISPLAY.PreRender();

        DISPLAY.WorldMode();
        render_world();

        DISPLAY.UiMode();
        render_ui();

        DISPLAY.PostRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void quit() {
    DISPLAY.Quit();
}

void init() {
    sim = std::make_shared<GravSim>();
    sim->Init();
}

int main() {

    init_display();
    init();
    run_loop();
    quit();
 
    // simulation_A();

    return 0;
}

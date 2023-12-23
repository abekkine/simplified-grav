#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Display.hpp"
#include "GravSim.hpp"

#include <thread>
#include <chrono>

class Application {
private:
    std::shared_ptr<GravSim> sim_;

public:
    Application() {}
    ~Application() {}

    void Init() {
        InitDisplay();
        sim_ = std::make_shared<GravSim>();
        sim_->Init();
    }
    void Run() {
        while (!DISPLAY.QuitCondition()) {
            DISPLAY.PreRender();

            DISPLAY.WorldMode();
            RenderWorld();

            DISPLAY.UiMode();
            RenderUi();

            DISPLAY.PostRender();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    void Quit() {
        DISPLAY.Quit();
    }

private:
    void InitDisplay() {
        DISPLAY.SetWindowSize(1280, 800);
        DISPLAY.SetWindowPosition(50, 50);
        DISPLAY.SetWindowTitle("gravsim");
        DISPLAY.SetCenter(0.0, 0.0);
        DISPLAY.SetPixelSize(30000000.0 / 800.0);
        DISPLAY.SetPanAndZoom(true);
        DISPLAY.Init();       
    }
    const double dT = 10.0 * 131.1 / 900.0;
    void RenderWorld() {
        sim_->Step(dT);
        sim_->RenderWorld();
    }
    void RenderUi() {
        sim_->RenderUi();
    }
};

#endif // APPLICATION_HPP

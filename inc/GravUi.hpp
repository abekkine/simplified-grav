#ifndef GRAV_UI_HPP
#define GRAV_UI_HPP

#include "GuiBase.hpp"

class GravUi : public GuiBase {
private:
    double var_elapsed_ = 0.0;
    double var_clock_ = 0.0;

public:
    GravUi() {}
    ~GravUi() {}
    void PreInit() override {}
    void PostInit() override {
        AddFont("version", "./fonts/Corbel.ttf", 16.0f);
        AddFont("ui", "./fonts/FiraCode.ttf", 24.0f);
    }
    void PreQuit() override {}
    void PostQuit() override {}
    void RenderWindows() override {
        RenderTest();
    }
    void RenderBackground() override {
        ShowVersionInfo();
    }
    void SetElapsed(const double& e) {
        var_elapsed_ = e;
    }
    void SetClock(const double& c) {
        var_clock_ = c;
    }

private:
    void RenderTest() {
        ImGui::Begin("Test");
        ImGui::Text("SimTime : %.1f", var_elapsed_);
        ImGui::Text("RealTime : %.1f", var_clock_);
        ImGui::End();
    }
    void ShowVersionInfo() {
        PushFont("version");
        std::string version = "n/a";
        RenderAt(version, 5, 5, IM_COL32_WHITE);
        PopFont();
    }
};

#endif // GRAV_UI_HPP

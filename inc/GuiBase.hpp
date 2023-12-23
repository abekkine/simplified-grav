#ifndef GUI_BASE_HPP
#define GUI_BASE_HPP

#include "CustomException.hpp"
#include "Display.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include <memory>
#include <string>
#include <unordered_map>

class GuiBase {
protected:
    ImGuiIO * io_;
    std::unordered_map<std::string, ImFont*> fonts_;

public:
    GuiBase() {}
    virtual ~GuiBase() {}
    virtual void PreInit() {}
    virtual void PostInit() {}
    virtual void PreQuit() {}
    virtual void PostQuit() {}
    virtual void RenderWindows() {}
    virtual void RenderBackground() {}

    bool UsesMouse() {
        return io_->WantCaptureMouse;
    }
    bool UsesKeyboard() {
        return io_->WantCaptureKeyboard;
    }
    void AddFont(const std::string & label, const std::string & path, const float size) {
        auto found = fonts_.find(label);
        if (found == fonts_.end()) {
            // Add new font.
            ImFont * font = io_->Fonts->AddFontFromFileTTF(path.c_str(), size);
            fonts_[label] = font;
        }
        else {
            // Throw since a font with the same name exists!
            throw CustomException(
                "Already added a font with [%s] label."
                , label.c_str()
            );
        }
    }
    void PushFont(const std::string & name) {
        auto found = fonts_.find(name);
        if (found != fonts_.end()) {
            ImGui::PushFont(fonts_[name]);
        } else {
            throw CustomException("No such font [%s] defined!", name.c_str());
        }
    }
    void PopFont() {
        ImGui::PopFont();
    }
    ImFont * GetFont(const std::string & name) {
        auto found = fonts_.find(name);
        if (found != fonts_.end()) {
            return fonts_[name];
        } else {
            return nullptr;
        }
    }
    void RenderAt(const std::string& message, const int x, const int y, const ImU32& color) {
        ImGui::GetWindowDrawList()->AddText(
            ImVec2(x, y),
            color,
            message.c_str()
        );
    }
    

    void Init() {
        // For derived class pre-initialization.
        PreInit();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io_ = &ImGui::GetIO();

        ImGui::LoadIniSettingsFromDisk("guiSetup.ini");

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(DISPLAY.GetWindow(), true);
        ImGui_ImplOpenGL2_Init();

        // For derived class post-initialization.
        PostInit();
    }
    void Quit() {
        // For derived class clean-up.
        PreQuit();

        // Cleanup
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        PostQuit();
    }
    void Step() {
        PreRender();
        // For derived class step functions.
        PreBackground();
        RenderBackground();
        PostBackground();

        RenderWindows();
        PostRender();
    }

private:
    void PreRender() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    void PostRender() {
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }
    void PreBackground() {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));
        ImGui::GetStyle().WindowPadding = ImVec2(0.0, 0.0);
        ImGui::SetNextWindowSize(io_->DisplaySize);
        ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
        ImGui::Begin("bg",
            NULL,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoBackground
        );
    }
    void PostBackground() {
        ImGui::End();
        ImGui::PopStyleVar();
    }
};

#endif // GUI_BASE_HPP

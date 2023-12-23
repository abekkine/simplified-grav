#ifndef DISPLAY_HPP_
#define DISPLAY_HPP_

#include "Logger.hpp"
#include "Viewport.hpp"
#include "Vector.hpp"

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#endif

#define DISPLAY Display::Instance()

typedef std::vector<std::string> StringList;

class Display {
public:
    enum {
        button__NONE = 0,
        button__LEFT = 1,
        button__MIDDLE = 2,
        button__RIGHT = 3,
        button__PRESSED = 4,
        button__RELEASED = 5,

        mod__NONE = 0,
        mod__SHIFT = 1,
        mod__CTRL = 2,
        mod__ALT = 4,
        mod__WIN = 8,
    };
private:
    // members
    GLFWwindow * window_;
    int width_;
    int height_;
    int window_x_, window_y_;
    std::string title_;
    bool full_screen_;
    Viewport viewport_;
    Vector cursor_;
    std::function<void(const int, const int)> key_cb_;
    std::function<void(const double &, const double &)> cursor_cb_, default_cursor_cb_;
    std::function<void(const int, const int, const int)> button_cb_, default_button_cb_;
    std::function<void(const double &)> scroll_cb_, default_scroll_cb_;
    std::function<void(StringList)> drop_cb_;

public:
    void PanZoomCursorHandler(const double& x, const double& y) {
        cursor_ = Vector(x, y);
        viewport_.Update(cursor_);
    }
    void PanZoomButtonHandler(int button, int action, int mods) {
        if (button == Display::button__RIGHT) {
            if (action == Display::button__PRESSED) {
                // If RMB is down -> Pan Start
                viewport_.PanStart(cursor_);
            } else {
                // If RMB is up -> Pan Stop
                viewport_.PanStop();
            }
        }
    }
    void PanZoomScrollHandler(const double& offset) {
        if (offset > 0) {
            // If wheel up -> Zoom in @ Cursor (>0)
            viewport_.ZoomInAtBy(cursor_, offset);
        }
        else if (offset < 0) {
            // If wheel down -> Zoom out @ Cursor (<0)
            viewport_.ZoomOutAtBy(cursor_, -offset);
        }
    }

public:
    // methods
    static Display & Instance() {
        static Display instance;
        return instance;
    }
    GLFWwindow *& GetWindow() {
        return window_;
    }
    void Init() {
        L_DEBUG("Display::Init()");

        if (!glfwInit()) {
            throw std::runtime_error("Unable to initialize GLFW!");
        }
        glfwWindowHint(GLFW_SAMPLES, 8);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        viewport_.SetWindowSize(width_, height_);
        viewport_.Update();
        if (full_screen_) {
            window_ = glfwCreateWindow(
                width_, height_,
                title_.c_str(),
                glfwGetPrimaryMonitor(),
                NULL
            );
        }
        else {
            window_ = glfwCreateWindow(
                width_, height_,
                title_.c_str(),
                NULL,
                NULL
            );
        }

        if (!window_) {
            Quit();
            throw std::runtime_error("Unable to create window!");
        }

        glfwSetWindowPos(window_, window_x_, window_y_);
        glfwMakeContextCurrent(window_);

        glfwSetKeyCallback(window_, key_callback);
        glfwSetCursorPosCallback(window_, cursor_callback);
        glfwSetMouseButtonCallback(window_, mouse_callback);
        glfwSetScrollCallback(window_, scroll_callback);
        glfwSetDropCallback(window_, drop_callback);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);
    }
    void SetPanAndZoom(const bool enable) {
        if (enable) {
            cursor_cb_ = std::bind(
                &Display::PanZoomCursorHandler,
                this,
                std::placeholders::_1,
                std::placeholders::_2
            );
            button_cb_ = std::bind(
                &Display::PanZoomButtonHandler,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
            );
            scroll_cb_ = std::bind(
                &Display::PanZoomScrollHandler,
                this,
                std::placeholders::_1
            );
        } else {
            cursor_cb_ = default_cursor_cb_;
            button_cb_ = default_button_cb_;
            scroll_cb_ = default_scroll_cb_;
        }
    }
    void SetWindowTitle(const std::string & title) {
        title_ = title;
    }
    void SetWindowPosition(const int x, const int y) {
        window_x_ = x;
        window_y_ = y;
    }
    void SetWindowSize(const int width, const int height) {
        L_DEBUG("Display::SetWindowSize(%d, %d)", width, height);

        width_ = width;
        height_ = height;
    }
    void SetPixelSize(const double & size) {
        L_DEBUG("Display::SetPixelSize(%f)", size);

        viewport_.SetPixelSize(size);
        viewport_.Update();
    }
    double GetPixelSize() {
        const double ps = viewport_.GetPixelSize();
        L_NONE("Display::GetPixelSize() => %f", ps);
        return ps;
    }
    void SetCenter(const double & x, const double & y) {
        L_NONE("Display::SetCenter(%f, %f)", x, y);
        viewport_.SetCenter(Vector(x, y));
        viewport_.Update();
    }
    void GetViewport(Viewport* & vp) {
        L_NONE("Display::GetViewport() => ..");
        vp = &viewport_;
    }
    void Quit() {
        L_DEBUG("Display::Quit()");
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
    bool QuitCondition() {
        return glfwWindowShouldClose(window_);
    }
    void RequestQuit() {
        L_DEBUG("Display::RequestQuit()");
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
    void Hide() {
        glfwIconifyWindow(window_);
    }
    void Show() {
        glfwRestoreWindow(window_);
    }
    void UiMode() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, width_, height_, 0.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
    }
    void WorldMode() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        viewport_.Ortho();
        glMatrixMode(GL_MODELVIEW);
    }
    void PreRender() {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void PostRender() {
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
    void RegisterKeyProcessor(std::function<void(const int, const int)> cb) {
        L_DEBUG("Display::RegisterKeyProcessor(..)");
        key_cb_ = cb;
    }
    void RegisterCursorProcessor(std::function<void(const double &, const double &)> cb) {
        L_DEBUG("Display::RegisterCursorProcessor(..)");
        cursor_cb_ = cb;
    }
    void RegisterMouseProcessor(std::function<void(const int, const int, const int)> cb) {
        L_DEBUG("Display::RegisterMouseProcessor(..)");
        button_cb_ = cb;
    }
    void RegisterScrollCallback(std::function<void(const double &)> cb) {
        L_DEBUG("Display::RegisterScrollCallback(..)");
        scroll_cb_ = cb;
    }
    void RegisterDropCallback(std::function<void(StringList)> cb) {
        L_DEBUG("Display::RegisterDropCallback(..)");
        drop_cb_ = cb;
    }

    void KeyCallback(const int key, const int mods) {
        // L_DEBUG("Display::KeyCallback(%d, %d)", key, mods);
        key_cb_(key, mods);
    }
    void CursorCallback(const double & x, const double & y) {
        cursor_cb_(x, y);
    }
    void MouseCallback(const int button_id, const int button_action, const int mods) {
        button_cb_(button_id, button_action, mods);
    }
    void ScrollCallback(const double & offset) {
        scroll_cb_(offset);
    }
    void DropCallback(StringList paths) {
        drop_cb_(paths);
    }

private:
    // methods
    Display()
    : window_(0)
    , width_(200)
    , height_(200)
    , window_x_(50)
    , window_y_(50)
    , title_("None")
    , full_screen_(false)
    {
        L_DEBUG("Display::C'tor()");
        key_cb_ = [] (const int key, const int mods) {
            // Quit if ESC pressed.
            if (key == GLFW_KEY_ESCAPE) {
                DISPLAY.RequestQuit();
            }
        };
        default_cursor_cb_ = [] (const double & x, const double & y) {
            (void)x;
            (void)y;
            // Do nothing.
        };
        default_button_cb_ = [] (const int id, const int action, const int mods) {
            (void)id;
            (void)action;
            (void)mods;
            // Do nothing.
        };
        default_scroll_cb_ = [] (const double & offset) {
            (void)offset;
            // Do nothing.
        };
        drop_cb_ = [] (StringList paths) {
            (void)paths;
            // Do nothing.
        };

        cursor_cb_ = default_cursor_cb_;
        button_cb_ = default_button_cb_;
        scroll_cb_ = default_scroll_cb_;
    }
    ~Display() {
        L_DEBUG("Display::D'tor()");
    }

private:
    // static callbacks
    static void key_callback(GLFWwindow * win, int key, int scancode, int action, int mods) {
        (void)win;
        (void)scancode;

        if (GLFW_RELEASE == action) {
            DISPLAY.KeyCallback(key, mods);
        }
    }
    static void cursor_callback(GLFWwindow * win, double x, double y) {
        (void)win;
        
        DISPLAY.CursorCallback(x, y);
    }
    static void mouse_callback(GLFWwindow * win, int button, int action, int mods) {
        (void)win;
        (void)mods;

        int button_action = button__NONE;
        switch (action) {
        case GLFW_PRESS:
            button_action = button__PRESSED; break;
        case GLFW_RELEASE:
            button_action = button__RELEASED; break;
        }

        int button_id = button__NONE;
        switch(button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            button_id = button__LEFT; break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            button_id = button__MIDDLE; break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            button_id = button__RIGHT; break;
        }

        DISPLAY.MouseCallback(button_id, button_action, mods);
    }
    static void scroll_callback(GLFWwindow * win, double xoffset, double yoffset) {
        (void)win;
        (void)xoffset;
        // Use only y-offset for mouse wheel scroll.
        DISPLAY.ScrollCallback(yoffset);
    }
    static void drop_callback(GLFWwindow * win, int count, const char** paths) {
        (void)win;
        StringList path_list;
        for (int i=0; i<count; ++i) {
            path_list.push_back(std::string(paths[i]));
        }
        DISPLAY.DropCallback(path_list);
    }
};

#endif // DISPLAY_HPP_

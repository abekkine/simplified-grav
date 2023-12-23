#ifndef VIEWPORT_HPP_
#define VIEWPORT_HPP_

#include "Vector.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#endif

#include <algorithm>

struct Viewport {
private:
    double left_, right_;
    double bottom_, top_;
    double near_, far_;
    Vector center_;
    double pixel_size_;
    int width_, height_;

public:
    Viewport() : Viewport(-1.0, 1.0, -1.0, 1.0) {}
    Viewport(
        const double & left, const double & right,
        const double & bottom, const double & top
    )
    : left_(left), right_(right)
    , bottom_(bottom), top_(top)
    , pixel_size_(1.0)
    , width_(256), height_(256)
    {
        near_ = -1.0;
        far_ = 1.0;
    }
    void SetCenter(const Vector & center) {
        center_ = center;
    }
    void SetWindowSize(const int width, const int height) {
        width_ = width;
        height_ = height;
    }
    void SetPixelSize(const double & pixel_size) {
        pixel_size_ = pixel_size;
    }
    double GetPixelSize() {
        return pixel_size_;
    }
    void GetCenter(Vector & center) {
        center = center_;
    }
    void GetExtent(Vector & topleft, Vector & bottomright) {
        topleft.x = left_;
        topleft.y = top_;
        bottomright.x = right_;
        bottomright.y = bottom_;
    }
    void Update() {
        const double half_width = double(width_) * pixel_size_ / 2.0;
        const double half_height = double(height_) * pixel_size_ / 2.0;
        left_ = center_.x - half_width;
        right_ = center_.x + half_width;
        bottom_ = center_.y - half_height;
        top_ = center_.y + half_height;
        L_INFO("vp[%.1f, %.1f, %.1f, %.1f]"
            , left_, right_
            , bottom_, top_
        );
    }
    void Update(const Vector & cursor) {
        Pan(cursor);
    }
    void Ortho() {
        glOrtho(
            left_, right_,
            bottom_, top_,
            near_, far_
        );
    }
    // Conversions
    Vector ToScreen(const Vector & world) {
        Vector pos;
        pos.x = width_ * (world.x - left_) / (right_ - left_);
        pos.y = height_ * (top_ - world.y) / (top_ - bottom_);
        return std::move(pos);
    }
    Vector ToWorld(const Vector & screen) {
        Vector pos;
        pos.x = left_ + screen.x * (right_ - left_) / width_;
        pos.y = top_ - screen.y * (top_ - bottom_) / height_;
        return std::move(pos);
    }
    // Pan feature
    Vector pan_start_;
    Vector pan_start_center_;
    bool pan_mode_;
    void PanStart(const Vector & p) {
        pan_mode_ = true;
        pan_start_ = p;
        pan_start_center_ = center_;
    }
    void PanStop() {
        pan_mode_ = false;       
    }
    void Pan(const Vector & cursor) {

        if (pan_mode_ == false) {
            return;
        }

        double dx = cursor.x - pan_start_.x;
        double dy = cursor.y - pan_start_.y;

        center_.x = pan_start_center_.x - dx * pixel_size_;
        center_.y = pan_start_center_.y + dy * pixel_size_;

        Update();
    }
    // End Pan Feature

    // Zoom Feature
    void ZoomInAtBy(const Vector & p, int count) {
        for (int i=0; i<count; ++i) {
            ZoomAt(p, 0.99);
        }
    }
    void ZoomOutAtBy(const Vector & p, int count) {
        for (int i=0; i<count; ++i) {
            ZoomAt(p, 1.01);
        }
    }
    void ZoomAt(const Vector & p, const double & ratio) {
        double pixel_size_0 = pixel_size_;
        pixel_size_ *= ratio;

        left_ += p.x * (pixel_size_0 - pixel_size_);
        top_ -= p.y * (pixel_size_0 - pixel_size_);
        right_ = left_ + width_ * pixel_size_;
        bottom_ = top_ - height_ * pixel_size_;

        center_.x = 0.5 * (left_ + right_);
        center_.y = 0.5 * (top_ + bottom_);
    }
    // End Zoom Feature
};

#endif // VIEWPORT_HPP_

#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include "Logger.hpp"

#include <iostream>
#include <array>
#include <cmath>

struct Vector {
    double x, y, z;

    Vector() : Vector(0.0, 0.0, 0.0) {}
    Vector(const double (&v)[3]) : Vector(v[0], v[1], v[2]) {}
    Vector(const std::array<double, 3> & v) : Vector(v[0], v[1], v[2]) {}
    Vector(const double & xv, const double & yv) : Vector(xv, yv, 0.0) {}
    Vector(const double & xv, const double & yv, const double & zv)
    : x(xv), y(yv), z(zv) {}

    void AssignTo(double * v) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }
    // void AssignTo(double (&v)[3]) {
    //     v[0] = x;
    //     v[1] = y;
    //     v[2] = z;
    // }
    void SetFrom(const double * v) {
        x = v[0];
        y = v[1];
        z = v[2];
    }
    // void SetFrom(const double (&v)[3]) {
    //     x = v[0];
    //     y = v[1];
    //     z = v[2];
    // }

    double DistanceTo(const Vector & to) const {
        const double dx = to.x - x;
        const double dy = to.y - y;
        return sqrt(dx * dx + dy * dy);
    }

    double Size() const {
        return sqrt(x * x + y * y + z * z);
    }

    void Normalize() {
        const double size = Size();
        if (size > 0.0) {
            x /= size;
            y /= size;
            z /= size;
        }
    }

    Vector Unit() {
        const double size = Size();
        Vector u;
        u = *this;
        if (size > 0.0) {
            u.Normalize();
        }
        return u;
    }

    Vector Rotate2D(const double & angle) {
        // Rotate around Z axis
        const double aRad = 3.141592653 * angle / 180.0;
        Vector r;
        r.x = x * cos(aRad) - y * sin(aRad);
        r.y = x * sin(aRad) + y * cos(aRad);
        r.z = z;
        return r;
    }

    void Dump(const std::string & name = "") {
        L_INFO("%s(%f, %f, %f)"
            , name.c_str()
            , x
            , y
            , z
        );
    }
    // TODO : define operations for vector A+B, A-B, AxB, A.B, nA.
    // TODO : create unit tests for all above.

    Vector operator+(const Vector & other) {
        Vector r;
        r.x = x + other.x;
        r.y = y + other.y;
        r.z = z + other.z;
        return r;
    }
    Vector operator-(const Vector & other) {
        Vector r;
        r.x = x - other.x;
        r.y = y - other.y;
        r.z = z - other.z;
        return r;
    }
    double operator*(const Vector & other) {
        return (x * other.x +
                y * other.y +
                z * other.z);
    }
    Vector operator*(const double & other) {
        Vector r;
        r.x = other * x;
        r.y = other * y;
        r.z = other * z;
        return r;
    }
    Vector operator^(const Vector & other) {
        Vector r;
        r.x = y * other.z - z * other.y;
        r.y = z * other.x - x * other.z;
        r.z = x * other.y - y * other.x;
        return r;
    }
};

Vector operator*(const double & b, const Vector & a);

#endif // VECTOR_HPP_

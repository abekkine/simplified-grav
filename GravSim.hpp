#ifndef GRAV_SIM_HPP
#define GRAV_SIM_HPP

#include "Display.hpp"

#include <cmath>
#include <cstdio>

class GravSim {
private:
    const double k_SunMass = 1.98855e30;
    const double k_EarthMass = 5.9722e24;
    const double k_GravitationalConstant = 6.67408e-11;
    const double k_AstronomicalUnit = 1.49598e11;
    const double k_EarthOrbitalSpeed = 2.9783e4;
    const double k_Pi = 3.14159265358979323846;
    const double k_Rad2Deg = 180.0 / k_Pi;
    const double k_Deg2Rad = k_Pi / 180.0;
    
    const double cDistanceFactor = 0.0001;
    const double cMassFactor = k_GravitationalConstant * 1e-6;
    const double cSpeedFactor = 0.01;
    const double cT = cDistanceFactor / cSpeedFactor;
    
    const double dt = cT;
    const double c_SunMass = k_SunMass * cMassFactor * cSpeedFactor * cSpeedFactor * cDistanceFactor;
    const double c_EarthMass = k_EarthMass * cMassFactor * cSpeedFactor * cSpeedFactor * cDistanceFactor;
    const double c_GravitationalConstant = k_GravitationalConstant / cMassFactor;
    const double c_AstronomicalUnit = k_AstronomicalUnit * cDistanceFactor;
    const double c_EarthOrbitalSpeed = k_EarthOrbitalSpeed * cSpeedFactor;
    
    double x_;
    double y_;
    double vx_;
    double vy_;
    double elapsed_;

public:
    GravSim() {}
    ~GravSim() {}
    void Init() {
        x_ = c_AstronomicalUnit;
        y_ = 0.0;
        vx_ = 0.0;
        vy_ = -c_EarthOrbitalSpeed;
        elapsed_ = 0.0;
    }
    void RenderWorld() {
        // Render Sun
        glPointSize(10.0);
        glColor3f(1.0, 1.0, 0.0);
        glBegin(GL_POINTS);
        glVertex2d(0.0, 0.0);
        glEnd();

        // Render Earth
        glPointSize(5.0);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_POINTS);
        glVertex2d(x_, y_);
        glEnd();
    }
    void RenderUi() {}
    void Step(const double& dt) {

        double a, r, ax, ay;

        const double r2 = x_ * x_ + y_ * y_;
        a = c_SunMass * c_GravitationalConstant / r2;
        r = sqrt(r2);
        ax = -a * x_ / r;
        ay = -a * y_ / r;
        vx_ += ax * dt;
        vy_ += ay * dt;
        x_ += vx_ * dt;
        y_ += vy_ * dt;
        elapsed_ += dt / (cT * 3600.0 * 24);

        // printf("(%.1f, %.1f)\n", x_, y_);
    }
};

#endif // GRAV_SIM_HPP


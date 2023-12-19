#include <cmath>
#include <cstdio>

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

void simulation_A() {
    // Initial.
    double x = c_AstronomicalUnit;
    double y = 0.0;
    double vx = 0.0;
    double vy = c_EarthOrbitalSpeed;

    // Loop
    int quit_count = 0;
    double a, r, ax, ay;
    double alpha = 0.0, alpha_old = 0.0;
    double elapsed = 0.0;
    double r_min = 1e30;
    double r_max = -1e30;
    while (quit_count < 5) {
        a = c_SunMass * c_GravitationalConstant / (x * x + y * y);
        r = sqrt(x * x + y * y);

        if (r < r_min) {
            r_min = r;
        }
        if (r > r_max) {
            r_max = r;
        }

        ax = -a * x / r;
        ay = -a * y / r;

        vx += ax * dt;
        vy += ay * dt;
        x += vx * dt;
        y += vy * dt;

        elapsed += dt;

        alpha = atan2(y, x) * k_Rad2Deg;
        if (alpha < 0.0) {
            alpha += 360.0;
        }
        const double d_alpha = alpha - alpha_old;
        if (d_alpha > 10.0) {
            alpha_old = alpha;
            printf(".");
        } else if (d_alpha < 0.0) {
            alpha_old = alpha;
            ++quit_count;
            puts("");
            printf("r_min(%.1f), r_max(%.1f), elapsed(%.1f)\n"
                , r_min * 0.001
                , r_max * 0.001
                , elapsed / (3600 * 24 * cT)
            );
        }

    }
    puts("===");
    printf("Sun Mass : %.1f kg\n", c_SunMass);
    printf("Earth Mass : %.1f kg\n", c_EarthMass);
    printf("Gravitational Constant : %.1f m^3 / (kg * s^2)\n", c_GravitationalConstant);
    printf("Astronomical Unit : %.1f km\n", c_AstronomicalUnit * 1e-3);
    printf("Earth Orbital Speed : %.1f km / s\n", c_EarthOrbitalSpeed * 1e-3);
    printf("Time Factor : %.4f\n", cT);
    printf("Speed of Light : %.1f km / s\n", cSpeedFactor * 299792458.0 * 1e-3);
}

int main() {

    simulation_A();

    return 0;
}
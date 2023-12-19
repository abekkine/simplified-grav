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
const double dt = 1;

void simulation_A() {
    // Initial.
    double x = k_AstronomicalUnit;
    double y = 0.0;
    double vx = 0.0;
    double vy = k_EarthOrbitalSpeed;

    // Loop
    int quit_count = 0;
    double a, r, ax, ay;
    double alpha = 0.0, alpha_old = 0.0;
    double elapsed = 0.0;
    double r_min = 1e30;
    double r_max = -1e30;
    while (quit_count < 5) {
        a = k_SunMass * k_GravitationalConstant / (x * x + y * y);
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
            // printf("%.1f, %.1f @ %.1f\n", alpha, r * 0.001, elapsed);
        } else if (d_alpha < 0.0) {
            alpha_old = alpha;
            ++quit_count;
            printf("%.1f, %.1f @ %.1f\n", alpha, r * 0.001, elapsed / (3600 * 24));
            printf("r_min = %.1f km\n", r_min * 0.001);
            printf("r_max = %.1f km\n", r_max * 0.001);
            puts("===");
        }
    }
}

int main() {

    simulation_A();

    return 0;
}
#include "tween.h"

// TODO(Dustin): Memoize the results
double ease(int type, double t) {
    double d = 1.0;
    double pi = 3.1415926535897932384626433832795;
    double pi2 = 3.1415926535897932384626433832795 / 2.0;

    double p = t / d;

    switch (type) {
        // Modeled after the line y = x
        default:
        case TWEEN_LINEAR: {
            return p;
        }

        // Modeled after the parabola y = x^2
        case TWEEN_QUADIN: {
            return p * p;
        }

        // Modeled after the parabola y = -x^2 + 2x
        case TWEEN_QUADOUT: {
            return -(p * (p - 2));
        }

        // Modeled after the piecewise quadratic
        // y = (1/2)((2x)^2)             ; [0, 0.5)
        // y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
        case TWEEN_QUADINOUT: {
            if (p < 0.5) {
                return 2 * p * p;
            } else {
                return (-2 * p * p) + (4 * p) - 1;
            }
        }

        // Modeled after the cubic y = x^3
        case TWEEN_CUBICIN: {
            return p * p * p;
        }

        // Modeled after the cubic y = (x - 1)^3 + 1
        case TWEEN_CUBICOUT: {
            double f = (p - 1);
            return f * f * f + 1;
        }

        // Modeled after the piecewise cubic
        // y = (1/2)((2x)^3)       ; [0, 0.5)
        // y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
        case TWEEN_CUBICINOUT: {
            if (p < 0.5) {
                return 4 * p * p * p;
            } else {
                double f = ((2 * p) - 2);
                return 0.5 * f * f * f + 1;
            }
        }

        // Modeled after the quartic x^4
        case TWEEN_QUARTIN: {
            return p * p * p * p;
        }

        // Modeled after the quartic y = 1 - (x - 1)^4
        case TWEEN_QUARTOUT: {
            double f = (p - 1);
            return f * f * f * (1 - p) + 1;
        }

        // Modeled after the piecewise quartic
        // y = (1/2)((2x)^4)        ; [0, 0.5)
        // y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
        case TWEEN_QUARTINOUT: {
            if (p < 0.5) {
                return 8 * p * p * p * p;
            } else {
                double f = (p - 1);
                return -8 * f * f * f * f + 1;
            }
        }

        // Modeled after the quintic y = x^5
        case TWEEN_QUINTIN: {
            return p * p * p * p * p;
        }

        // Modeled after the quintic y = (x - 1)^5 + 1
        case TWEEN_QUINTOUT: {
            double f = (p - 1);
            return f * f * f * f * f + 1;
        }

        // Modeled after the piecewise quintic
        // y = (1/2)((2x)^5)       ; [0, 0.5)
        // y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
        case TWEEN_QUINTINOUT: {
            if (p < 0.5) {
                return 16 * p * p * p * p * p;
            } else {
                double f = ((2 * p) - 2);
                return 0.5 * f * f * f * f * f + 1;
            }
        }

        // Modeled after quarter-cycle of sine wave
        case TWEEN_SINEIN: {
            return sin((p - 1) * pi2) + 1;
        }

        // Modeled after quarter-cycle of sine wave (different phase)
        case TWEEN_SINEOUT: {
            return sin(p * pi2);
        }

        // Modeled after half sine wave
        case TWEEN_SINEINOUT: {
            return 0.5 * (1 - cos(p * pi));
        }

        // Modeled after shifted quadrant IV of unit circle
        case TWEEN_CIRCIN: {
            return 1 - sqrt(1 - (p * p));
        }

        // Modeled after shifted quadrant II of unit circle
        case TWEEN_CIRCOUT: {
            return sqrt((2 - p) * p);
        }

        // Modeled after the piecewise circular function
        // y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
        // y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
        case TWEEN_CIRCINOUT: {
            if (p < 0.5) {
                return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
            } else {
                return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
            }
        }

        // Modeled after the exponential function y = 2^(10(x - 1))
        case TWEEN_EXPOIN: {
            return (p == 0.0) ? p : pow(2, 10 * (p - 1));
        }

        // Modeled after the exponential function y = -2^(-10x) + 1
        case TWEEN_EXPOOUT: {
            return (p == 1.0) ? p : 1 - pow(2, -10 * p);
        }

        // Modeled after the piecewise exponential
        // y = (1/2)2^(10(2x - 1))         ; [0,0.5)
        // y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
        case TWEEN_EXPOINOUT: {
            if (p == 0.0 || p == 1.0) return p;

            if (p < 0.5) {
                return 0.5 * pow(2, (20 * p) - 10);
            } else {
                return -0.5 * pow(2, (-20 * p) + 10) + 1;
            }
        }

        // Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x -
        // 1))
        case TWEEN_ELASTICIN: {
            return sin(13 * pi2 * p) * pow(2, 10 * (p - 1));
        }

        // Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2,
        // -10x) + 1
        case TWEEN_ELASTICOUT: {
            return sin(-13 * pi2 * (p + 1)) * pow(2, -10 * p) + 1;
        }

        // Modeled after the piecewise exponentially-damped sine wave:
        // y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
        // y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
        case TWEEN_ELASTICINOUT: {
            if (p < 0.5) {
                return 0.5 * sin(13 * pi2 * (2 * p)) *
                       pow(2, 10 * ((2 * p) - 1));
            } else {
                return 0.5 * (sin(-13 * pi2 * ((2 * p - 1) + 1)) *
                                  pow(2, -10 * (2 * p - 1)) +
                              2);
            }
        }

        // Modeled (originally) after the overshooting cubic y = x^3-x*sin(x*pi)
        case TWEEN_BACKIN: { /*
            return p * p * p - p * sin(p * pi); */
            double s = 1.70158f;
            return p * p * ((s + 1) * p - s);
        }

        // Modeled (originally) after overshooting cubic y =
        // 1-((1-x)^3-(1-x)*sin((1-x)*pi))
        case TWEEN_BACKOUT: { /*
            double f = (1 - p);
            return 1 - (f * f * f - f * sin(f * pi)); */
            double s = 1.70158f;
            return --p, 1.f * (p * p * ((s + 1) * p + s) + 1);
        }

        // Modeled (originally) after the piecewise overshooting cubic function:
        // y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
        // y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
        case TWEEN_BACKINOUT: { /*
            if(p < 0.5) {
                double f = 2 * p;
                return 0.5 * (f * f * f - f * sin(f * pi));
            }
            else {
                double f = (1 - (2*p - 1));
                return 0.5 * (1 - (f * f * f - f * sin(f * pi))) + 0.5;
            } */
            double s = 1.70158f * 1.525f;
            if (p < 0.5) {
                return p *= 2, 0.5 * p * p * (p * s + p - s);
            } else {
                return p = p * 2 - 2, 0.5 * (2 + p * p * (p * s + p + s));
            }
        }

#define tween$bounceout(p)                                                    \
    ((p) < 4 / 11.0                                                           \
         ? (121 * (p) * (p)) / 16.0                                           \
         : (p) < 8 / 11.0                                                     \
               ? (363 / 40.0 * (p) * (p)) - (99 / 10.0 * (p)) + 17 / 5.0      \
               : (p) < 9 / 10.0 ? (4356 / 361.0 * (p) * (p)) -                \
                                      (35442 / 1805.0 * (p)) + 16061 / 1805.0 \
                                : (54 / 5.0 * (p) * (p)) -                    \
                                      (513 / 25.0 * (p)) + 268 / 25.0)

        case TWEEN_BOUNCEIN: {
            return 1 - tween$bounceout(1 - p);
        }

        case TWEEN_BOUNCEOUT: {
            return tween$bounceout(p);
        }

        case TWEEN_BOUNCEINOUT: {
            if (p < 0.5) {
                return 0.5 * (1 - tween$bounceout(1 - p * 2));
            } else {
                return 0.5 * tween$bounceout((p * 2 - 1)) + 0.5;
            }
        }

#undef tween$bounceout

        case TWEEN_SINESQUARE: {
            double A = sin((p)*pi2);
            return A * A;
        }

        case TWEEN_EXPONENTIAL: {
            return 1 / (1 + exp(6 - 12 * (p)));
        }

        case TWEEN_SCHUBRING1: {
            return 2 * (p + (0.5f - p) * abs(0.5f - p)) - 0.5f;
        }

        case TWEEN_SCHUBRING2: {
            double p1pass = 2 * (p + (0.5f - p) * abs(0.5f - p)) - 0.5f;
            double p2pass =
                2 * (p1pass + (0.5f - p1pass) * abs(0.5f - p1pass)) - 0.5f;
            double pAvg = (p1pass + p2pass) / 2;
            return pAvg;
        }

        case TWEEN_SCHUBRING3: {
            double p1pass = 2 * (p + (0.5f - p) * abs(0.5f - p)) - 0.5f;
            double p2pass =
                2 * (p1pass + (0.5f - p1pass) * abs(0.5f - p1pass)) - 0.5f;
            return p2pass;
        }

        case TWEEN_SWING: {
            return ((-cos(pi * p) * 0.5) + 0.5);
        }

        case TWEEN_SINPI2: {
            return sin(p * pi2);
        }
    }
}
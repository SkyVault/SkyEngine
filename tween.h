#ifndef BENIS_TWEEN_H
#define BENIS_TWEEN_H

enum TweenType {
    TWEEN_LINEAR,
    TWEEN_QUADIN,  // t^2
    TWEEN_QUADOUT,
    TWEEN_QUADINOUT,
    TWEEN_CUBICIN,  // t^3
    TWEEN_CUBICOUT,
    TWEEN_CUBICINOUT,
    TWEEN_QUARTIN,  // t^4
    TWEEN_QUARTOUT,
    TWEEN_QUARTINOUT,
    TWEEN_QUINTIN,  // t^5
    TWEEN_QUINTOUT,
    TWEEN_QUINTINOUT,
    TWEEN_SINEIN,  // sin(t)
    TWEEN_SINEOUT,
    TWEEN_SINEINOUT,
    TWEEN_EXPOIN,  // 2^t
    TWEEN_EXPOOUT,
    TWEEN_EXPOINOUT,
    TWEEN_CIRCIN,  // sqrt(1-t^2)
    TWEEN_CIRCOUT,
    TWEEN_CIRCINOUT,
    TWEEN_ELASTICIN,  // exponentially decaying sine wave
    TWEEN_ELASTICOUT,
    TWEEN_ELASTICINOUT,
    TWEEN_BACKIN,  // overshooting cubic easing: (s+1)*t^3 - s*t^2
    TWEEN_BACKOUT,
    TWEEN_BACKINOUT,
    TWEEN_BOUNCEIN,  // exponentially decaying parabolic bounce
    TWEEN_BOUNCEOUT,
    TWEEN_BOUNCEINOUT,
    TWEEN_SINESQUARE,   // gapjumper's
    TWEEN_EXPONENTIAL,  // gapjumper's
    TWEEN_SCHUBRING1,   // terry schubring's formula 1
    TWEEN_SCHUBRING2,   // terry schubring's formula 2
    TWEEN_SCHUBRING3,   // terry schubring's formula 3
    TWEEN_SINPI2,       // tomas cepeda's
    TWEEN_SWING,        // tomas cepeda's & lquery's
    TWEEN_TOTAL,
    TWEEN_UNDEFINED
};

double ease(int type, double t);

#endif  // BENIS_TWEEN_H
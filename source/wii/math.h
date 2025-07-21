#pragma once

extern "C" {

/**
 * Returns the absolute value of x.
 */
inline double fabs(double x)
{
    return __builtin_fabs(x);
}

/**
 * Returns the absolute value of x.
 */
inline float fabsf(float x)
{
    return __builtin_fabs(x);
}

} // extern "C"
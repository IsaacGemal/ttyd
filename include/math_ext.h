#pragma once

#include "math_trig.h"

// Out-of-line MSL math entry points used by the game. The SDK math.h substitutes
// intrinsics and float sqrt inlines; these callers require the external ABI.
double sqrt(double x);
double hypot(double x, double y);
double sin(double x);
double cos(double x);
double pow(double x, double y);
double tan(double arg);
double fabs(double x);
float fabsf(float x);
double fmod(double x, double y);
double floor(double x);
float __fabsf(float x);
double __fabs(double x);
double acos(double x);
double asin(double x);
double log(double x);
unsigned __cvt_fp2unsigned(double d);

#pragma once

#define M_E		2.7182818284590452354f
#define M_LOG2E		1.4426950408889634074f
#define M_LOG10E	0.43429448190325182765f
#define M_LN2		0.69314718055994530942f
#define M_LN10		2.30258509299404568402f
#define M_PI		3.14159265358979323846f
#define M_PI_2		1.57079632679489661923f
#define M_PI_4		0.78539816339744830962f
#define M_1_PI		0.31830988618379067154f
#define M_2_PI		0.63661977236758134308f
#define M_2_SQRTPI	1.12837916709551257390f
#define M_SQRT2		1.41421356237309504880f
#define M_SQRT1_2	0.70710678118654752440f


/*
#define M_E		2.718281f//8284590452354f
#define M_LOG2E		1.442695f//0408889634074f
#define M_LOG10E	0.434294f//48190325182765f
#define M_LN2		0.693147f//18055994530942f
#define M_LN10		2.302585f//09299404568402f
#define M_PI		3.141592f//65358979323846f
#define M_PI_2		1.570796f//32679489661923f
#define M_PI_4		0.785398f//16339744830962f
#define M_1_PI		0.318309f//88618379067154f
#define M_2_PI		0.636619f//77236758134308f
#define M_2_SQRTPI	1.128379f//16709551257390f
#define M_SQRT2		1.414213f//56237309504880f
#define M_SQRT1_2	0.707106f//78118654752440f
*/

#ifdef __cplusplus
extern "C" {
#endif
/*
double fmod(double val1, double val2);
double sin(double angle);
double cos(double angle);
double fabs(double value);
double sqrt(double value);
double floor(double value);
double ceil(double value);
double pow(double x, double y);
double exp(double x);
double log(double x);
double log10(double x);
*/
float fmodf(float val1, float val2);
float sinf(float angle);
float cosf(float angle);
float fabsf(float value);
float sqrtf(float value);
float floorf(float value);
float ceilf(float value);
float powf(float x, float y);
float expf(float x);
float logf(float x);
float log10f(float x);

#ifdef __cplusplus
}
#endif

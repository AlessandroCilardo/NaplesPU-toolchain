#include <stdint.h>
#include <math.h>

#define MAX_DELTA_DOUBLE 1.0E-15f

float MathPow_Float_Int(float x, int n);

//
// Standard library math functions
//

float fmodf(float val1, float val2) {
    int whole = val1 / val2;
    return val1 - (whole * val2);
}

//
// Use taylor series to approximate sine
//   x - x**3/3! + x**5/5! - x**7/7! ...
//

static const int kNumTerms = 6;

static const float kDenominators[] = {
    -0.166666666666667f,  // 1 / 3!
    0.008333333333333f,   // 1 / 5!
    -0.000198412698413f,  // 1 / 7!
    0.000002755731922f,	  // 1 / 9!
    -2.50521084e-8f,      // 1 / 11!
    1.6059044e-10f        // 1 / 13!
};


float sinf(float angle) {
    angle = fmodf(angle, M_PI * 2);
    int resultSign;
    if (angle < 0)
        resultSign = -1;
    else
        resultSign = 1;

    angle = fabsf(angle);
    if (angle > M_PI * 3 / 2)
    {
        angle = M_PI * 2 - angle;
        resultSign = -resultSign;
    }
    else if (angle > M_PI)
    {
        angle -= M_PI;
        resultSign = -resultSign;
    }
    else if (angle > M_PI / 2)
        angle = M_PI - angle;

    float angleSquared = angle * angle;
    float numerator = angle;
    float result = angle;

    for (int i = 0; i < kNumTerms; i++)
    {
        numerator *= angleSquared;
        result += numerator * kDenominators[i];
    }

    return result * resultSign;
}


float cosf(float angle) {
    return sinf(angle + M_PI_2);
}


float sqrtf(float value) {
    if(value < 0){
        return __builtin_nanf("");
    }
    float guess = value;
    for (int iteration = 0; iteration < 10; iteration++)
        guess = ((value / guess) + guess) / 2.0f;

    return guess;
}


float floorf(float value) {
    return (int) value;
}

float ceilf(float value) {
    float floorval = floorf(value);
    if (value > floorval)
        return floorval + 1.0f;

    return floorval;
}

float powf(float x, float y) {
    float ret;
    if ((x == 0.0f) && (y == 0.0f)) {
        return __builtin_nanf("");
    }
    else if ((x == 1.0f) || (y == 1.0f)) {
        ret = x;
    } else if (y < 0.0f) {
        ret = 1.0f / powf(x, -y);
    } else {
        ret = expf(y * logf(x));
    }
    return (ret);
}

float expf(float x) {
    float ret;
    if (x == 1.0f) {
        ret = M_E;
    } else if (x < 0.0f) {
        ret = 1.0f / expf(-x);
    } else {
        int n = 2;
        float d;
        ret = 1.0f + x;
        do {
            d = x;
            for (int i = 2; i <= n; i++) {
                d *= x / i;
            }
            ret += d;
            n++;
        } while (d > MAX_DELTA_DOUBLE);
    }
    return (ret);
}


float logf(float x) {
    float ret = 0.0f, d;
    if(x<0.0f)
        return __builtin_nanf("");
    else if (x==0.0f)
        return -(__builtin_inff());
    else {
        int n = 1;
        do {
            int a = 2 * n - 1;
            d = (1.0f / a) * MathPow_Float_Int((x - 1.0f) / (x + 1.0f), a);
            ret += d;
            n++;
        } while (fabsf(d) > MAX_DELTA_DOUBLE);
    }
    return (ret * 2.0f);
}


float log10f( float x ) {
    return logf(x) / M_LN10;    
}

float MathPow_Float_Int(float x, int n) {
    float ret;
    if ((x == 1.0f) || (n == 1)) {
        ret = x;
    } else if (n < 0) {
        n=-n;
        ret = 1.0f;
        while (n--) {
            ret *= x;
        }
        ret = 1.0f / ret;
    } else {
        ret = 1.0f;
        while (n--) {
            ret *= x;
        }
    }
    return (ret);
}


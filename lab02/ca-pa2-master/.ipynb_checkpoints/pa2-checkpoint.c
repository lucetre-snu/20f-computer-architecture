//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2020)
//
//  Project #2: FP12 (12-bit floating point) Representation
//
//  September 28, 2020
//
//  Injae Kang (abcinje@snu.ac.kr)
//  Sunmin Jeong (sunnyday0208@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------
#include "pa2.h"

/* Convert 32-bit signed integer to 12-bit floating point */
fp12 int_fp12(int n)
{
    int sign = 0, exp = 0, frac = 0;
    if (n == 0) return 0;
    if (n < 0) {
        sign = 31;
        n = -n;
    }
    for (; n >> (exp+1) && exp < 31; exp++);
    
    int X = exp>=7 ? n & ((1 << (exp-6)) - 1) : 0;
    int R = exp>=6 ? n & (1 << (exp-6)) : 0;
    int L = exp>=5 ? n & (1 << (exp-5)) : 0;
    
    frac = (unsigned) n >> (exp-5);
    
    if (R && X) frac++;
    else if (L && R && !X) frac += (frac & 1);

    if (frac & (1 << 6)) {
        exp++;
        frac >>= 1;
    }
    frac &= 31;
    exp = (exp + 31) & 63;
//     PRINT(uint16_t, "fp12", frac);
//     PRINT(uint16_t, "fp12", exp);
    
	return (sign << 11) + (exp << 5) + (frac);
}

/* Convert 12-bit floating point to 32-bit signed integer */
int fp12_int(fp12 x)
{
    int sign = (31<<11) & x;
    int exp = ((unsigned) ((63<<5) & x) >> 5);
    
    if (exp == 63) return 1 << 31;
    else exp -= exp ? 31 : 30;
    
    int frac = (31) & x;
    //PRINT(uint16_t, "fp12", exp);
    int n = exp>0 ? (1 << exp) : 0;
    for (int i = 0; i < 5 && exp-i > 0; i++)
        n += (frac & (1<<(4-i))) ? 1 << (exp-i-1) : 0;
    
	return sign ? -n : n;
}

union FloatingPointIEEE754 {
	struct {
		unsigned int frac : 23;
		unsigned int exp : 8;
		unsigned int sign : 1;
	} raw;
	float f;
};

union FloatingPoint12 {
	struct {
		unsigned int frac : 5;
		unsigned int exp : 6;
		unsigned int sign : 5;
	} raw;
	fp12 f;
};

/* Convert 32-bit single-precision floating point to 12-bit floating point */
fp12 float_fp12(float f)
{
    union FloatingPointIEEE754 x;
    union FloatingPoint12 y;
    x.f = f;

    y.raw.sign = x.raw.sign ? 31 : 0;
    
    if (x.raw.exp == 255) {
        y.raw.exp = 63;
        y.raw.frac = x.raw.frac ? 31 : 0;
        return y.f;
    }
    int exp = x.raw.exp ? x.raw.exp-127 : x.raw.exp-126;
    unsigned int frac = x.raw.frac >> 18;
    
    int L = x.raw.frac & (1<<18) ? 1 : 0;
    int R = x.raw.frac & (1<<17) ? 1 : 0;
    int X = x.raw.frac & ((1<<17)-1);
    
    if (R && X) frac++;
    else if (L && R && !X) frac += (frac & 1);
    
    if (frac & (1 << 6)) {
        exp++;
        frac >>= 1;
    }
    if (exp > 31) {
        y.raw.exp = 63;
        y.raw.frac = 0;
        return y.f;
    }
    else if (exp < -30) {
        y.raw.exp = 0;
        y.raw.frac = 0;
        return y.f;   
    }
    y.raw.exp = exp + 31;
    y.raw.frac = frac & 31;
    return y.f;
}

/* Convert 12-bit floating point to 32-bit single-precision floating point */
float fp12_float(fp12 f)
{
    union FloatingPoint12 x;
    union FloatingPointIEEE754 y;
    x.f = f;

    y.raw.sign = x.raw.sign ? 1 : 0;
    
    if (x.raw.exp == 63) {
        y.raw.exp = 255;
        y.raw.frac = x.raw.frac ? (1<<17) - 1 : 0;
        return y.f;
    }
    else if (x.raw.exp) {
        y.raw.exp = x.raw.exp - 31 + 127;
        y.raw.frac = x.raw.frac << 18;
        return y.f;
    }
    int exp = x.raw.exp - 30;
    unsigned int frac = x.raw.frac;
    int i = 0;
    for (; i < 5; i++)
        if (frac & (1<<(4-i))) {
            frac &= (1<<(4-i)) - 1;
            break;   
        }
    if (i < 5) {
        y.raw.exp = exp - (i+1) + 127;
        y.raw.frac = frac << i+1;
    }
    else y.raw.exp = 0;
   // PRINT(uint16_t, "fp12", y.raw.frac);
    return y.f;
}

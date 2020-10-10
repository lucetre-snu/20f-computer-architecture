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




/* Convert 32-bit signed integer to 12-bit floating point */
fp12 int_fp12(int n)
{
    union Int {
        struct {
            unsigned int num : 31;
            unsigned int sign : 1;
        } raw;
        int i;
    } x;

    union FloatingPoint12 y;
    
    x.i = n;
    y.f = 0;
    
    if (n == 0) return 0;
    if (x.raw.sign) {
        x.i = -n;
        y.raw.sign = 31;
        if (x.raw.sign) return 0xffc0;
    }
    
    int exp = 0;
    unsigned int num = x.raw.num, frac = 0;
    for (exp = 0; num >> (exp+1); exp++);
    
    int L = 0, R = 0, X = 0;
    if (exp >= 5) {
        L = num & (1 << (exp-5));
        if (exp > 5) {
            R = num & (1 << (exp-6));
            X = num & ((1 << (exp-6)) - 1);
        }
    }
    
    frac = num >> (exp-5);    
    if (R && X) frac++;
    else if (L && R && !X) frac += (frac & 1);

    if (frac & (1 << 6)) {
        exp++;
        frac >>= 1;
    }
    
    y.raw.frac = frac & 31;
    y.raw.exp = (exp + 31) & 63;
    
	return y.f;
}

/* Convert 12-bit floating point to 32-bit signed integer */
int fp12_int(fp12 f)
{
    union FloatingPoint12 x;
    
    x.f = f;    
    if (x.raw.exp >= 62) return 1 << 31;
    
    unsigned int frac = x.raw.frac, n = 0;
    int exp = x.raw.exp + (x.raw.exp ? 0 : 1) - 31;
    
    if (exp >= 0) {
        if (exp >= 5) frac <<= exp-5;
        else frac >>= 5-exp;
        n = (1 << exp) + frac;
    }
    
    return x.raw.sign ? -n : n;
}

/* Convert 32-bit single-precision floating point to 12-bit floating point */
fp12 float_fp12(float f)
{
    union FloatingPointIEEE754 x;
    union FloatingPoint12 y;
    
    x.f = f;
    y.f = 0;
    y.raw.sign = x.raw.sign ? 31 : 0;
    
    int exp = x.raw.exp - 127;
    if (exp > 31) {
        y.raw.exp = 63;
        y.raw.frac = (x.raw.exp == 255) && x.raw.frac ? 1 : 0;
        return y.f;
    }
    else if (exp < -36) {
        y.raw.exp = 0;
        y.raw.frac = 0;
        return y.f;
    }
    else if (exp > -31) {
        unsigned int frac = x.raw.frac | (1 << 23);
        union {
            struct {
                unsigned int X : 17;
                unsigned int R : 1;
                unsigned int L : 1;
            } raw;
            unsigned int frac;
        } lrx = { .frac = frac};
        frac >>= 18;
        
        if (lrx.raw.R && (lrx.raw.X || (lrx.raw.L && !lrx.raw.X)) ) {
            frac++;
            if (frac > 63) {
                y.raw.exp = exp+1 + 31;
                y.raw.frac = 0;
                return y.f;
            }
        }
        y.raw.exp = exp + 31;
        y.raw.frac = frac & 31;
        return y.f;
    }
    else {
        unsigned int frac = x.raw.frac | (1 << 23);
        struct {
            unsigned int X;
            unsigned int R;
            unsigned int L;
        } lrx = {
            .X = frac & ((1<<(-exp-13)) - 1),
            .R = (frac & (1<<(-exp-13))) ? 1 : 0,
            .L = (frac & (1<<(-exp-12))) ? 1 : 0,
        };
        frac >>= (-exp-12);
        
        if (lrx.R && (lrx.X || (lrx.L && !lrx.X)) ) {
            frac++;
            if (frac > 31) {
                y.raw.exp = 1;
                y.raw.frac = 0;
                return y.f;
            }
        }
        y.raw.exp = 0;
        y.raw.frac = frac & 31;
        return y.f;   
    }
}

/* Convert 12-bit floating point to 32-bit single-precision floating point */
float fp12_float(fp12 f)
{
    union FloatingPoint12 x;
    union FloatingPointIEEE754 y;
    x.f = f;
    y.f = 0;
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
        y.raw.frac = frac << (19+i);
    }
    else y.raw.exp = 0;
    return y.f;
}

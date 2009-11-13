/** 
 * @file dSFMT-sse2.h 
 * @brief double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
 * for Intel SSE2.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#ifndef DSFMT_SSE2_H
#define DSFMT_SSE2_H

#include <emmintrin.h>

static __m128i sse2_param_mask;
static __m128i sse2_low_mask;
static __m128i sse2_high_const;
static __m128i sse2_int_one;
static __m128d sse2_double_two;
static __m128d sse2_double_m_one;

static void setup_const(void);

#if defined(__GNUC__)
inline static __m128i mm_recursion(__m128i *a, __m128i *b, __m128i c, __m128i d)
    __attribute__((always_inline));
#elif defined(_MSC_VER) && _MSC_VER >= 1200
__forceinline static __m128i mm_recursion(__m128i *a, __m128i *b, __m128i c,
				   __m128i d);
#else
inline static __m128i mm_recursion(__m128i *a, __m128i *b, __m128i c,
				   __m128i d);
#endif
/**
 * This function setup some constant variables for SSE2.
 */
static void setup_const(void) {
    static int first = 1;
    if (!first) {
	return;
    }
    sse2_param_mask = _mm_set_epi32(SFMT_MSK32_3, SFMT_MSK32_4,
				    SFMT_MSK32_1, SFMT_MSK32_2);
    sse2_low_mask = _mm_set_epi32(SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2,
				  SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2);
    sse2_int_one = _mm_set_epi32(0, 1, 0, 1);
    sse2_high_const = _mm_set_epi32(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0);
    sse2_double_two = _mm_set_pd(2.0, 2.0);
    sse2_double_m_one = _mm_set_pd(-1.0, -1.0);
    first = 0;
}

/**
 * This function represents the recursion formula.
 * @param a a 128-bit part of the internal state array
 * @param b a 128-bit part of the internal state array
 * @param c a 128-bit part of the internal state array
 * @param d a 128-bit part of the internal state array
 * @return next value of recursion
 */
inline static __m128i mm_recursion(__m128i *a, __m128i *b,
				   __m128i c, __m128i d) {
    __m128i v, w, x, y, z;
    
    z = _mm_load_si128(a);
    y = _mm_srli_epi64(*b, SFMT_SR1);
    y = _mm_and_si128(y, sse2_param_mask);
    w = _mm_slli_epi64(c, SFMT_SL1);
    x = _mm_srli_epi64(c, SFMT_SR2);
    v = _mm_shuffle_epi32(d, SSE2_SHUFF);
    w = _mm_xor_si128(w, x);
    v = _mm_xor_si128(v, z);
    z = _mm_slli_si128(z, SFMT_SL2);
    w = _mm_xor_si128(w, y);
    v = _mm_xor_si128(v, z);
    v = _mm_xor_si128(v, w);
    v = _mm_and_si128(v, sse2_low_mask);
    /*v = _mm_or_si128(v, sse2_high_const);*/
    return v;
}

/**
 * This function converts the double precision floating point numbers which
 * distribute uniformly in the range [1, 2) to those which distribute uniformly
 * in the range [0, 1).
 * @param array array of double precision floating point numbers
 * @param size size of the array
 */
inline static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

/**
 * This function converts the double precision floating point numbers which
 * distribute uniformly in the range [1, 2) to those which distribute uniformly
 * in the range (0, 1].
 * @param array array of double precision floating point numbers
 * @param size size of the array
 */
inline static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].sd = _mm_sub_pd(sse2_double_two, array[i].sd);
    }
}

/**
 * This function converts the double precision floating point numbers which
 * distribute uniformly in the range [1, 2) to those which distribute uniformly
 * in the range (0, 1).
 * @param array array of double precision floating point numbers
 * @param size size of the array
 */
inline static void convert_oo(w128_t array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
	array[i].si = _mm_or_si128(array[i].si, sse2_int_one);
	array[i].sd = _mm_add_pd(array[i].sd, sse2_double_m_one);
    }
}

/**
 * This function fills the internal state array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 */
inline static void gen_rand_all(void) {
    int i;
    __m128i r, lung;

    lung = _mm_load_si128(&sfmt[SFMT_N].si);
    r = _mm_load_si128(&sfmt[SFMT_N - 1].si);
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + SFMT_POS1].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&sfmt[i].si, r);
    }
    for (; i < SFMT_N; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + SFMT_POS1 - SFMT_N].si, r,
			 lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&sfmt[i].si, r);
    }
    _mm_store_si128(&sfmt[SFMT_N].si, lung);
}

/**
 * This function fills the user-specified array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pseudorandom numbers to be generated.
 */
inline static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    __m128i r, lung;

    lung = _mm_load_si128(&sfmt[SFMT_N].si);
    r = _mm_load_si128(&sfmt[SFMT_N - 1].si);
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	r = mm_recursion(&sfmt[i].si, &sfmt[i + SFMT_POS1].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
    }
    for (; i < SFMT_N; i++) {
	r = mm_recursion(&sfmt[i].si, &array[i + SFMT_POS1 - SFMT_N].si, r,
			 lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
    }
    /* main loop */
    for (; i < size - SFMT_N; i++) {
	r = mm_recursion(&array[i - SFMT_N].si,
			 &array[i + SFMT_POS1 - SFMT_N].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
    }
    for (j = 0; j < 2 * SFMT_N - size; j++) {
	sfmt[j].si = array[j + size - SFMT_N].si;
    }    
    for (; i < size; i++, j++) {
	r = mm_recursion(&array[i - SFMT_N].si,
			 &array[i + SFMT_POS1 - SFMT_N].si, r, lung);
	lung = _mm_xor_si128(lung, r);
	r = _mm_or_si128(r, sse2_high_const);
	_mm_store_si128(&array[i].si, r);
	_mm_store_si128(&sfmt[j].si, r);
    }
    _mm_store_si128(&sfmt[SFMT_N].si, lung);
}
#endif	/* DSFMT_SSE2_H */

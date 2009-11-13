/** 
 * @file dSFMT-alti.h 
 * @brief double precision SIMD-oriented Fast Mersenne Twister (dSFMT)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#ifndef DSFMT_ALTI_H
#define DSFMT_ALTI_H
/**
 * This function represents the recursion formula.
 * @param a position of array.
 * @param b position of array.
 * @param reg position of array which may keep in register.
 * @param lung special posion of array which may keep in register.
 * @return next value of recursion
 */
inline static vector unsigned int vec_recursion(vector unsigned int a,
						vector unsigned int b,
						vector unsigned int reg,
						vector unsigned int lung) {
    vector unsigned int r, s, t, u, v, w, x, y, z;
    const vector unsigned char sl1 = ALTI_SL1;
    const vector unsigned char sl1_perm = ALTI_SL1_PERM;
    const vector unsigned int sl1_msk = ALTI_SL1_MSK;
    const vector unsigned char sl2_perm = ALTI_SL2_PERM;
    const vector unsigned char sr1 = ALTI_SR1;
    const vector unsigned int sr1_msk = ALTI_SR1_MSK;
    const vector unsigned char sr2_perm = ALTI_SR2_PERM;
    const vector unsigned char perm = ALTI_PERM;
    const vector unsigned int low_mask = ALTI_LOW_MSK;
    /* const vector unsigned int high_const = ALTI_HIGH_CONST;*/

    x = vec_perm(a, (vector unsigned int)sl2_perm, sl2_perm);
    y = vec_srl(b, sr1);
    y = vec_and(y, sr1_msk);
    z = vec_perm(reg, (vector unsigned int)sl1_perm, sl1_perm);
    z = vec_sll(z, sl1);
    z = vec_and(z, sl1_msk);
    w = vec_perm(reg, (vector unsigned int)sr2_perm, sr2_perm);
    v = vec_perm(lung, (vector unsigned int)perm, perm);
    s = vec_xor(a, x);
    t = vec_xor(y, z);
    u = vec_xor(w, v);
    r = vec_xor(s, t);
    r = vec_xor(r, u);
    r = vec_and(r, low_mask);
    /* r = vec_or(r, high_const);*/
    return r;
}

/**
 * This function fills the internal state array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 */
inline static void gen_rand_all(void) {
    int i;
    vector unsigned int r, lung;
    const vector unsigned int high_const = ALTI_HIGH_CONST;

    lung = sfmt[SFMT_N].s;
    r = sfmt[SFMT_N - 1].s;
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + SFMT_POS1].s, r, lung);
	lung = vec_xor(lung, r);
	r = vec_or(r, high_const);
	sfmt[i].s = r;
    }
    for (; i < SFMT_N; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + SFMT_POS1 - SFMT_N].s, r, lung);
	lung = vec_xor(lung, r);
	r = vec_or(r, high_const);
	sfmt[i].s = r;
    }
    sfmt[SFMT_N].s = lung;
}

/**
 * This function fills the user-specified array with double precision
 * floating point pseudorandom numbers of the IEEE 754 format.
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pseudorandom numbers to be generated.
 */
inline static void gen_rand_array(w128_t array[], int size) {
    int i, j;
    vector unsigned int r, lung;
    const vector unsigned int high_const = ALTI_HIGH_CONST;

    /* read from sfmt */
    lung = sfmt[SFMT_N].s;
    r = sfmt[SFMT_N - 1].s;
    for (i = 0; i < SFMT_N - SFMT_POS1; i++) {
	r = vec_recursion(sfmt[i].s, sfmt[i + SFMT_POS1].s, r, lung);
	lung = vec_xor(lung, r);
	r = vec_or(r, high_const);
	array[i].s = r;
    }
    for (; i < SFMT_N; i++) {
	r = vec_recursion(sfmt[i].s, array[i + SFMT_POS1 - SFMT_N].s, r, lung);
	lung = vec_xor(lung, r);
	r = vec_or(r, high_const);
	array[i].s = r;
    }
    /* main loop */
    for (; i < size - SFMT_N; i++) {
	r = vec_recursion(array[i - SFMT_N].s, array[i + SFMT_POS1 - SFMT_N].s,
			  r, lung);
	lung = vec_xor(lung, r);
	r = vec_or(r, high_const);
	array[i].s = r;
    }
    for (j = 0; j < 2 * SFMT_N - size; j++) {
	sfmt[j].s = array[j + size - SFMT_N].s;
    }
    for (; i < size; i++) {
	r = vec_recursion(array[i - SFMT_N].s, array[i + SFMT_POS1 - SFMT_N].s,
			  r, lung);
	lung = vec_xor(lung, r);
	r = vec_or(r, high_const);
	array[i].s = r;
	sfmt[j++].s = r;
    }
    sfmt[SFMT_N].s = lung;
}

#endif	/* DSFMT_ALTI_H*/

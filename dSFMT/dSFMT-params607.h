#ifndef DSFMT_PARAMS607_H
#define DSFMT_PARAMS607_H

#define SFMT_POS1	2
#define SFMT_SL1	33
#define SFMT_SL2	1
#define SFMT_SR1	7
#define SFMT_SR2	24
#define SFMT_MSK1	UINT64_C(0xffcfeef7fdffffff)
#define SFMT_MSK2	UINT64_C(0xfdffffb7ffffffff)
#define SFMT_MSK32_1	0xffcfeef7U
#define SFMT_MSK32_2	0xfdffffffU
#define SFMT_MSK32_3	0xfdffffb7U
#define SFMT_MSK32_4	0xffffffffU
#define SFMT_PCV1	UINT64_C(0x0000000000000001)
#define SFMT_PCV2	UINT64_C(0x0005196200000000)
#define SFMT_IDSTR \
	"dSFMT-607:2-33-1-7-24:ffcfeef7fdffffff-fdffffb7ffffffff"


/* PARAMETERS FOR ALTIVEC */
#if defined(__APPLE__)	/* For OSX */
    #define ALTI_SL1 	(vector unsigned int)(1, 1, 1, 1)
    #define ALTI_SL1_PERM \
	(vector unsigned char)(4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3)
    #define ALTI_SL1_MSK \
	(vector unsigned int)(0xfffffffeU,0x00000000U,0xfffffffeU,0x00000000U)
    #define ALTI_SL2_PERM \
	(vector unsigned char)(1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0)
    #define ALTI_SR1 \
	(vector unsigned int)(SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1)
    #define ALTI_SR1_MSK \
	(vector unsigned int)(0x01cfeef7U,0xfdffffffU,0x01ffffb7U,0xffffffffU)
    #define ALTI_SR2_PERM \
	(vector unsigned char)(19,19,19,0,1,2,3,4,19,19,19,8,9,10,11,12)
    #define ALTI_PERM \
	(vector unsigned char)(8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7)
    #define ALTI_LOW_MSK \
	(vector unsigned int)(SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \
		SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2)
    #define ALTI_HIGH_CONST \
	(vector unsigned int)(SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0)
#else	/* For OTHER OSs(Linux?) */
    #define ALTI_SL1 	{1, 1, 1, 1}
    #define ALTI_SL1_PERM \
	{4,5,6,7,28,28,28,28,12,13,14,15,0,1,2,3}
    #define ALTI_SL1_MSK \
	{0xfffffffeU,0x00000000U,0xfffffffeU,0x00000000U}
    #define ALTI_SL2_PERM \
	{1,2,3,4,5,6,7,31,9,10,11,12,13,14,15,0}
    #define ALTI_SR1 \
	{SFMT_SR1, SFMT_SR1, SFMT_SR1, SFMT_SR1}
    #define ALTI_SR1_MSK \
	{0x01cfeef7U,0xfdffffffU,0x01ffffb7U,0xffffffffU}
    #define ALTI_SR2_PERM \
	{19,19,19,0,1,2,3,4,19,19,19,8,9,10,11,12}
    #define ALTI_PERM \
	{8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7}
    #define ALTI_LOW_MSK \
	{SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2, \
		SFMT_LOW_MASK32_1, SFMT_LOW_MASK32_2}
    #define ALTI_HIGH_CONST \
	{SFMT_HIGH_CONST32, 0, SFMT_HIGH_CONST32, 0}
#endif

#endif /* DSFMT_PARAMS607_H */

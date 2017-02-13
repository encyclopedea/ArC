/*	Macros for bit twiddling	*/

#ifndef BIT_TWIDDLE
#define	BIT_TWIDDLE

#define SELECT_BIT(b, x)	( (x) & (0x1 << (b)) )
#define	SELECT_BIT_FRONT(b, x)	( (x) & (0x1 << (sizeof(x) * 8 - (b))) )

#endif
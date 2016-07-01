
#ifndef __XORSHIFT32__
#define __XORSHIFT32__

#if _WIN32
#ifndef uint32_t	
	typedef unsigned long uint32_t;
#endif
#else
	uint32_t	y;
#endif


class xorshift32
{
protected:


	uint32_t	y;


public:
	xorshift32(){}
	~xorshift32(){}

	void	init_genrand(unsigned long seed) {y=seed;}

	uint32_t genrand_uint32(){
		y = y ^ ( y << 13 );
		y = y ^ ( y >> 17 );
		y = y ^ ( y << 15 );

		return y;
	}

	float genrand_float32() {
		uint32_t v = genrand_uint32();
		uint32_t res = (v >> 9) | 0x3f800000;
		float	r = (*(float*)&res) - 1.0f;

		return r;
	}

} ;




#endif

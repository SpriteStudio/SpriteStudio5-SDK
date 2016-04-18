
#ifndef __XORSHIFT32__
#define __XORSHIFT32__



class xorshift32
{
protected:
    unsigned long y;
public:
	xorshift32(){}
	~xorshift32(){}

	void	init_genrand(unsigned long seed) {y=seed;}

	unsigned long genrand_uint32(){
		y = y ^ ( y << 13 );
		y = y ^ ( y >> 17 );
		y = y ^ ( y << 15 );

		return y;
	}

	float genrand_float32() {
		unsigned long v = genrand_uint32();
		unsigned res = (v >> 9) | 0x3f800000;
		return (*(float*)&res) - 1.0f;
	}

} ;




#endif

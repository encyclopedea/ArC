#ifndef AREN_INCLUDED
#define AREN_INCLUDED

#include <ostream>
#include <stdint.h>

class Model;

class ArEncoder{
public:
	ArEncoder(Model* m, std::ostream* out);
	~ArEncoder();

	bool put(uint8_t c);
	int finish();
private:
	Model* m;
	std::ostream* out;
	uint32_t buf;
	int pending;
	int bufcurs;
	uint32_t top;
	uint32_t bot;

	inline bool outputBits(uint32_t bits, int count);
	inline bool outputBit(uint8_t c);
	inline int outputPending(uint8_t c);

	inline void removeFirstConvergence();
	inline void removeSecondConvergence();
};

#endif
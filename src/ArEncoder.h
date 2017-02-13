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
	uint8_t buf;
	int bufcurs;
	int pending;
	uint32_t top;
	uint32_t bot;

	inline bool outputBit(uint8_t c);
	inline int outputPending(uint8_t c);
};

#endif
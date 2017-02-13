#ifndef ARDE_INCLUDED
#define ARDE_INCLUDED

#include <istream>
#include <stdint.h>

class Model;

// Flags
const char STREAM_NULL		= 0x1;
const char MODEL_NULL		= 0x2;
const char STREAM_NOT_GOOD	= 0x4;


class ArDecoder{
public:
	ArDecoder(Model* m, std::istream* in);
	~ArDecoder();

	uint8_t get();
	uint8_t getFlags();
private:
	Model* m;
	std::istream* in;
	uint8_t buf;
	int bufcurs;
	uint8_t flags;
	uint32_t top;
	uint32_t bot;
	uint32_t cur;

	inline char getBit();
};

#endif
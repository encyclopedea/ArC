#ifndef ARDE_INCLUDED
#define ARDE_INCLUDED

#include <istream>
#include <stdint.h>

class Model;

// Flags
const char STREAM_NULL		= 0b001;
const char MODEL_NULL		= 0b010;
const char STREAM_NOT_GOOD	= 0b100;


class ArDecoder{
public:
	ArDecoder(Model* m, std::istream* in);
	~ArDecoder();

	uint8_t get();
	bool good();
private:
	Model* m;
	std::istream* in;
	uint8_t buf;
	int bufcurs;
	uint8_t flags;
	uint32_t top;
	uint32_t bot;
	uint32_t cur;

	char getBit();
};

#endif
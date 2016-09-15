#include <istream>

#include "Model.h"

// Flags
const char STREAM_NULL		= 0b001;
const char MODEL_NULL		= 0b010;
const char STREAM_NOT_GOOD	= 0b100;


class ArDecoder{
public:
	ArDecoder(Model* m, std::istream* in);
	~ArDecoder();

	unsigned char get();
	bool good();
private:
	Model* m;
	std::istream* in;
	unsigned char buf;
	int bufcurs;
	char flags;
	unsigned int top;
	unsigned int bot;
	unsigned int cur;

	char getBit();
};
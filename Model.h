#pragma once

#include <iosfwd>
#include <stdint.h>

class Bitstream;

class Model{
public:
	Model();
	~Model(){}

	bool update(uint8_t c);
	bool update(uint8_t c, int count);
	void digest();

	uint32_t calcUpper(uint8_t c, uint32_t bot, uint32_t top);
	uint32_t calcLower(uint8_t c, uint32_t bot, uint32_t top);

	uint8_t getChar(uint32_t enc, uint32_t bot, uint32_t top);
	uint32_t getTotal();
	uint32_t getCharCount(uint8_t c);

	void reset();

	void exportModel(std::ostream& out);
	void importModel(std::istream& in);

private:
	uint32_t freqs[256]; // range of uint_8
	uint32_t total;
	bool digested;

	void undigest();
};
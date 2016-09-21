#include "ArDecoder.h"
#include "proto.h"
#include "Model.h"

#include <iostream>
#include <bitset>//tmp


ArDecoder::ArDecoder(Model* m, std::istream* in){
	this->m = m;
	this->in = in;

	buf = 0;
	bufcurs = 7;

	flags = 0;

	if (m == NULL){
		flags |= MODEL_NULL;
	}
	if (in == NULL){
		flags |= STREAM_NULL;
	}

	if (!flags){
		char* c = reinterpret_cast<char*>(&cur);
		for (int i = TYPESIZE_BYTES - 1; i >= 0; i--){
			c[i] = in->get();
		}
		std::cout << cur << std::endl;
	}

	top = ~0;
	bot = 0;
}

ArDecoder::~ArDecoder(){}

uint8_t ArDecoder::get(){ //third round of test has issues
	if (flags & MODEL_NULL){
		return 0;
	}

	// std::cout << "Cur: " << cur << std::endl;
	uint8_t c = m->getChar(cur, bot, top);

	uint32_t tmp = top;
	top = m->calcUpper(c, bot, top);
	bot = m->calcLower(c, bot, tmp);

	// std::cout << "top: " << std::bitset<32>(top) << std::endl;
	// std::cout << "bot: " << std::bitset<32>(bot) << std::endl;

	// While the first bit of top and bot are the same
	while ((0x1 << (TYPESIZE - 1)) & ~(top ^ bot)){
		// std::cout << "Converged\n";
		top <<= 1;
		top |= 0x1;
		// Left shift bot, loading a 0 in
		bot <<= 1;
		// Left shift cur, loading a new encoded bit in
		cur <<= 1;
		cur |= getBit() & 0x1;
	}

	// While the second bit of bot is 1 and of top is 0
	while ((0x1 << (TYPESIZE_BYTES - 2)) & top < (0x1 << (TYPESIZE_BYTES - 2)) & bot){
		// Remove the second bit of top
		top = (top << 1) | (1 << (TYPESIZE_BYTES - 1));
		// Remove the second bit of bot
		bot = (bot << 1) & !(1 << (TYPESIZE_BYTES - 1));
	}

	return c;
}

/*
 * Returns the internal flags. 
 * If STREAM_NULL or MODEL_NULL are set, all get calls will fail.
 * If STREAM_NOT_GOOD is set, get calls may continue, but may result in 
 * undefined behavior after an unspecified number of get calls.
 */
bool ArDecoder::good(){
	return flags;
}

/*
 * Gets a bit from the internal buffer. If the internal buffer is emptied,
 * reads a new buffer from in and sets a flag if this fails.
 *
 * If a flag is set when getBit is called, it will fail and return 0;
 *
 * Assumes that in is not null.
 */
char ArDecoder::getBit(){
	if (flags){
		return 0;
	}

	char ret = (buf >> bufcurs) & 0b1;

	if (--bufcurs < 0){
		if (in->good()){
			buf = in->get();
		} else{
			flags |= STREAM_NOT_GOOD;
		}
	}

	return ret;
}
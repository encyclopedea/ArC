#include "ArDecoder.h"
#include "proto.h"
#include "Model.h"
#include <iostream> // tmp
#include <bitset> //tmp

ArDecoder::ArDecoder(Model* m, std::istream* in){
	this->m = m;
	this->in = in;

	buf = 0;
	bufcurs = 0;

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
	}

	top = ~0;
	bot = 0;
} 

ArDecoder::~ArDecoder(){}

uint8_t ArDecoder::get(){
	if (flags & MODEL_NULL){
		return 0x04;
	}

	uint8_t c = m->getChar(cur, bot, top);

	uint32_t tmp = top;
	top = m->calcUpper(c, bot, top);
	bot = m->calcLower(c, bot, tmp);

	std::cout << "bot before all: " << std::bitset<32>(bot) << std::endl;
	std::cout << "top before all: " << std::bitset<32>(top) << std::endl;
	std::cout << "cur before all: " << std::bitset<32>(cur) << std::endl;


	// While the first bit of top and bot are the same
	while ((0x1 << (TYPESIZE - 1)) & ~(top ^ bot)){
		// Discard the first bit of top, bot, cur
		std::cout << "Discarding " << (top >> (TYPESIZE - 1)) << std::endl;

		// Load 1 into top
		top <<= 1;
		top |= 0x1;

		// Load 0 into bot
		bot <<= 1;

		// Load bit from stream into cur
		cur <<= 1;
		cur |= getBit() & 0x1;
	}

	std::cout << "bot before convergence: " << std::bitset<32>(bot) << std::endl;
	std::cout << "top before convergence: " << std::bitset<32>(top) << std::endl;
	std::cout << "cur before convergence: " << std::bitset<32>(cur) << std::endl;

	// While the second bit of bot is 1 and of top is 0
	while (((0x1 << (TYPESIZE - 2)) & top) < ((0x1 << (TYPESIZE - 2)) & bot)){
		std::cout << "Removing convergence\n";
		// Remove the second bit of top and load a 1 in back
		top = (top << 1) | (1 << (TYPESIZE - 1));
		top |= 0x1;
		// Remove the second bit of bot and load a 0 in back
		bot = (bot << 1) & ~(1 << (TYPESIZE - 1));

		// 1stbit = cur & (cur & (0x1 << (TYPESIZE - 1)))
		// shift cur, wipe 1st bit, then 
		cur = ((cur << 1) & ~(1 << (TYPESIZE - 1))) | (cur & (0x1 << (TYPESIZE - 1)));
		cur |= getBit() & 0x1;
	}

	std::cout << "  " << cur << " in";
	std::cout << "\n[ " << bot << ", " << top << " )\n";

	std::cout << "bot after all: " << std::bitset<32>(bot) << std::endl;
	std::cout << "top after all: " << std::bitset<32>(top) << std::endl;
	std::cout << "cur after all: " << std::bitset<32>(cur) << std::endl;
		std::cout << std::endl;

	return c;
}

/*
 * Returns the internal flags. 
 * If STREAM_NULL or MODEL_NULL are set, all get() calls will fail.
 * If STREAM_NOT_GOOD is set, get() calls may continue, but may result in 
 * undefined behavior after an unspecified number of get calls.
 */
uint8_t ArDecoder::getFlags(){
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

	if (bufcurs-- < 1){
		if (in->good()){
			buf = in->get();
			bufcurs = 7;
		} else{
			flags |= STREAM_NOT_GOOD;
			return 0;
		}
	}

	char ret = (buf >> bufcurs) & 0b1;

	return ret;
}

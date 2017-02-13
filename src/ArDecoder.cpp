#include "ArDecoder.h"
#include "Model.h"
#include "bitTwiddle.h"


ArDecoder::ArDecoder(Model* model, std::istream* instream){
	m = model;
	in = instream;

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
		for (int i = sizeof(cur) - 1; i >= 0; i--){
			c[i] = in->get();
		}
	}

	top = ~0;
	bot = 0;
} 

ArDecoder::~ArDecoder(){}

uint8_t ArDecoder::get(){
	if (flags & MODEL_NULL){
		return 0;
	}

	uint8_t c = m->getChar(cur, bot, top);

	uint32_t tmp = top;
	top = m->calcUpper(c, bot, top);
	bot = m->calcLower(c, bot, tmp);

	removeFirstConvergence();
	removeSecondConvergence();

	return c;
}

inline void ArDecoder::removeFirstConvergence(){
	// While the first bit of top and bot are the same
	while (SELECT_BIT_FRONT(1, ~(top ^ bot))){
		// Discard the first bit of top, bot, cur

		// Load 1 into top
		top <<= 1;
		top |= 0x1;

		// Load 0 into bot
		bot <<= 1;

		// Load bit from stream into cur
		cur <<= 1;
		cur |= getBit() & 0x1;
	}
}

inline void ArDecoder::removeSecondConvergence(){
	// While the second bit of bot is 1 and of top is 0
	while (SELECT_BIT_FRONT(2, top) < SELECT_BIT_FRONT(2, bot)){

		// Remove the second bit of top and load a 1 in the back
		top = (top << 1) | (1 << (sizeof(top) * 8 - 1));
		top |= 0x1;

		// Remove the second bit of bot and leave a 0 in back
		bot = (bot << 1) & ~(1 << (sizeof(bot) * 8 - 1));

		// Remove the second bit of bot
		cur <<= 1;	// Second bit is opposite of first bit so don't lose it
		cur ^= 0x1 << (sizeof(cur) * 8 - 1);	// Restore the first bit by inverting the new first bit
		cur |= getBit() & 0x1;
	}
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
inline char ArDecoder::getBit(){
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

	return (buf >> bufcurs) & 0x1;
}
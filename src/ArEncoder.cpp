#include "ArEncoder.h"
#include "Model.h"
#include "bitTwiddle.h"

ArEncoder::ArEncoder(Model* model, std::ostream* outstream){
	m = model;
	out = outstream;

	bufcurs = sizeof(buf) * 8 - 1;
	buf = 0;
	pending = 0;

	top = ~0;
	bot = 0;
}

ArEncoder::~ArEncoder(){}

/*
 * Encodes a character. 
 * If m or out are NULL, returns false and does not encode. 
 * Otherwise, returns true.
 */
bool ArEncoder::put(uint8_t c){
	if (m == NULL || out == NULL){
		return false;
	}

	uint32_t tmp = top;
	top = m->calcUpper(c, bot, top);
	bot = m->calcLower(c, bot, tmp);

	removeFirstConvergence();
	removeSecondConvergence();

	return true;
}

#include <iostream>
inline void ArEncoder::removeFirstConvergence(){
	// Remove front matching bits
	int count = __builtin_clz(top ^ bot);
	if (count > 0){
		outputBit(top >> (sizeof(top) * 8 - 1));
		outputPending(top >> (sizeof(top) * 8 - 1));
		if (count > 1){
			outputBits(top >> (sizeof(top) * 8 - count), count - 1); // TODO outputting wrong?
		}
		top <<= count;
		top |= (1 << count) - 1;

		bot <<= count;
	}


}

inline void ArEncoder::removeSecondConvergence(){
	// While the second bit of bot is 1 and of top is 0
	while (SELECT_BIT_FRONT(2, top) < SELECT_BIT_FRONT(2, bot)){
		pending++;

		// Remove the second bit of top and load a 1 in the back
		top = (top << 1) | (1 << (sizeof(top) * 8 - 1));
		top |= 1;

		// Remove the second bit of bot and leave a 0 in the back
		bot = (bot << 1) & ~(1 << (sizeof(bot) * 8 - 1));
	}
}

/*
 * Outputs the buffer, bot, and all pending bits.
 * Pending bits can be either 0 or 1 depending on whether the range
 * converges towards bot or top, so since bot is used here, pending
 * bits are treated as 1s.
 *
 * If out is NULL, returns -1. Otherwise, returns the number of 
 * bits that were output.
 */
int ArEncoder::finish(){
	if (out == NULL){
		return -1;
	}

	int ret = sizeof(bot) * 8 + pending + sizeof(buf) * 8 - 1 - bufcurs;

	// First bit of bot is always 0 - otherwise it would have converged
	outputBit(0);
	outputPending(0);

	// Output the rest of bot
	bool cleared = false;
	for(int i = 30; i >= 0; i--){
		cleared = outputBit((bot >> i) & 0x1);
	}

	if (!cleared){
		out->write((char*) &buf, sizeof(buf));
		buf = 0;
		bufcurs = sizeof(buf) * 8 - 1;
	}

	return ret;
}

/*
 * Performs a buffered output. Uses only the rightmost bit of c.
 * Returns true if the buffer was output, false otherwise.
 *
 * Since it is private, it assumes that error checking on out
 * has already been done if it is being called.
 */
inline bool ArEncoder::outputBit(uint8_t c){
	bool ret = false;
	buf |= ((uint32_t)c & 0x1) << bufcurs;
	bufcurs--;

	if (bufcurs < 0){
		out->write((char*) &buf, sizeof(buf));
		buf = 0;
		bufcurs = sizeof(buf) * 8 - 1;
		ret = true;
	}

	return ret;
}

inline bool ArEncoder::outputBits(uint32_t bits, int count){
	bool ret = false;

	// Determine the number that will fit normally
	int fit = (bufcurs + 1 < count) ? bufcurs + 1 : count;
	uint32_t mask = (1 << (fit)) - 1;
	buf |= (mask & (bits >> (count - fit))) << (bufcurs - fit + 1);
	bufcurs -= fit;


	// Output if necessary
	if (bufcurs < 0){
		out->write((char*) &buf, sizeof(buf));
		buf = 0;
		bufcurs = sizeof(buf) * 8 - 1;
		ret = true;

		// Shove the rest in
		if (count - fit > 0){
			mask = (1 << (count - fit)) - 1;	// Mask for remainder
			buf |= (mask & bits) << (bufcurs - (count - fit) + 1);	// Put the remainder in
			bufcurs -= count - fit;	// Track the cursor
		}
	}

	// Do not need to output again (added max 31 bits in second round)

	return ret;
}

/*
 * Outputs the pending bits as the inverse of the rightmost bit of c.
 *
 * This is a private function so it is assumed that out has been
 * NULL checked if it is called.
 */
inline int ArEncoder::outputPending(uint8_t c){
	int ret = pending;
	while(pending > 0){
		outputBit(~c & 0x1);
		pending--;
	}
	
	return ret;
}

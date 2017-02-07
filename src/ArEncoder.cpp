#include "ArEncoder.h"
#include "proto.h"
#include "Model.h"

ArEncoder::ArEncoder(Model* m, std::ostream* out){
	this->m = m;
	this->out = out;

	bufcurs = 7;
	buf = 0;
	pending = 0;

	top = 0xffffffff;
	bot = 0x0;
}

ArEncoder::~ArEncoder(){}

/*
 * Encodes a character. 
 * If m and out are NULL, returns false. Otherwise, returns true.
 */
bool ArEncoder::put(uint8_t c){
	if (m == NULL || out == NULL){
		return false;
	}

	uint32_t tmp = top;
	top = m->calcUpper(c, bot, top);
	bot = m->calcLower(c, bot, tmp);

	// While the first bit of top and bot are the same
	while ((0x1 << (TYPESIZE - 1)) & ~(top ^ bot)){
		outputBit(top >> (TYPESIZE - 1));
		outputPending(top >> (TYPESIZE - 1));

		// Left shift top, loading a 1 in
		top <<= 1;
		top |= 0x1;
		// Left shift bot, loading a 0 in
		bot <<= 1;
	}

	// While the second bit of bot is 1 and of top is 0
	while ((0x1 << (TYPESIZE - 2)) & top < (0x1 << (TYPESIZE - 2)) & bot){
		pending++;
		// Remove the second bit of top
		top = (top << 1) | (1 << (TYPESIZE- 1));
		// Remove the second bit of bot
		bot = (bot << 1) & !(1 << (TYPESIZE- 1));
	}

	return true;
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

	int ret = TYPESIZE + pending + 7 - bufcurs;

	// First bit of bot is always 0 - otherwise it would have converged
	outputBit(0);
	outputPending(0);

	// Output the rest of bot
	bool cleared = false;
	for(int i = 30; i >= 0; i--){
		cleared = outputBit((bot >> i) & 0b1);
	}

	if (!cleared){
		out->put(buf);
		buf = 0;
		bufcurs = 7;
	}

	return ret;
}

/*
 * Performs a buffered output. Uses only the 0th bit of c.
 * Returns true if the buffer was output, false otherwise.
 *
 * Since it is private, it assumes that error checking on out
 * has already been done if it is being called.
 */
bool ArEncoder::outputBit(uint8_t c){
	bool ret = false;
	buf |= (c & 0b1) << bufcurs;
	bufcurs--;

	if (bufcurs < 0){
		out->put(buf);
		buf = 0;
		bufcurs = 7;
		ret = true;
	}

	return ret;
}

/*
 * Outputs the pending bits as the inverse of the 0th bit of c.
 *
 * This is a private function so it is assumed that out has been
 * NULL checked if it is called.
 */
int ArEncoder::outputPending(uint8_t c){
	int ret = pending;
	while(pending > 0){
		outputBit(~c & 0b1);
		pending--;
	}
	return ret;
}
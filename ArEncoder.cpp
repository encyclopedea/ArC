#include "ArEncoder.h>"

ArEncoder::ArEncoder(Model* m, std::ostream* out){
	this->m = m;
	this->out = out;
	bufcurs = 7;
	buf = 0;
	pending = 0;
}

ArEncoder::~ArEncoder(){
	finish(); // TODO: leave this here or nah?
}

/*
 * Encodes a character. 
 * If m and out are NULL, returns false. Otherwise, returns true.
 */
bool ArEncoder::put(unsigned char c){
	if (m == NULL || out == NULL){
		return false;
	}

	//TODO

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

	int ret = sizeof(uint) * 8 + pending + 7 - bufcurs;

	// First bit of bot is always 0 - otherwise it would have converged
	output(0);
	outputPending(0);

	// Output the rest of bot
	bool cleared = false;
	for(int i = 31; i >= 0; i--){
		output((bot >> i) & 0b1);
	}

	if (!cleared){
		out.put(buf);
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
bool ArEncoder::outputBit(char c){
	bool ret = false;
	buf |= (c & 0b1) << bufcurs;
	bufcurs--;

	if (bufcurs < 0){
		out.put(buf);
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
int outputPending(char c){
	int ret = pending;
	while(pending > 0){
		outputBit(~c & 0b1);
		pending--;
	}
	return ret;
}
#include "ArDecoder.h"

ArDecoder::ArDecoder(Model* m, std::istream* in){
	this->m = m;
	this->in = in;

	buf = 0;
	cursor = 7;

	flags = 0;

	if (m == NULL){
		flags |= MODEL_NULL;
	}
	if (in == NULL){
		flags |= STREAM_NULL;
	}
}

ArDecoder::~ArDecoder(){}

unsigned char get(){
	//TODO
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
 * Assumes that 
 */
char getBit(){
	if (flags){
		return 0;
	}

	char ret = (buf >> bufcurs) & 0b1;

	if (--bufcurs < 0){
		if (in.good()){
			in.get(buf);
		} else{
			flags |= STREAM_NOT_GOOD;
		}
	}

	return ret;
}
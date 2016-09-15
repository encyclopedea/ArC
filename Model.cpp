#include "Model.h"
#include <iostream>

Model::Model(){
	for (int i = 0; i < 257; i++){
		freqs[i] = 0;
	}
}

/*
 * Adds a character to the model. 
 *
 * If the model has already been digested, this takes additional
 * time.
 */
bool Model::update(unsigned char c){
	update(c, 1);
}

bool Model::update(unsigned char c, int count){
	freqs[c] += count;		// Increment the character's frequency
	total += count;			// Increment the total size

	if (digested){
		// Increment all further entries
		for (int i = c + 1; i < 256; i++){
			freqs[i] += count;
		}
	}
}

/*
 * Digests the current model. 
 * Digestion is required for most of the other member functions
 * to operate.
 *
 * After digestion, update() takes additional time.
 */
void Model::digest(){
	if (digested){
		return;
	}

	digested = true;

	// Accumulate the frequencies
	for (int i = 1; i < 256; i++){
		freqs[i] += freqs[i - 1];
	}
}

/*
 * Reverts a model to its pre-digest form.
 *
 * This is useful for exporting the model or speeding up sequential
 * updates.
 */
void Model::undigest(){
	if (!digested){
		return;
	}

	digested = false;

	// Decumulate the frequencies
	for (int i = 255; i > 0; i--){
		freqs[i] -= freqs[i - 1];
	}
}

/*
 * Calculates the upper bound of c, given the restrictions top and bot.
 *
 * If the model has not already been digested, calcUpper() digests it 
 * before doing calculations.
 */
uint Model::calcUpper(unsigned char c, uint bot, uint top){
	if (!digested){
		digest();
	}

	// Determine the current range
	int range = top - bot;

	// Scale the top of the character onto the current range
	int num = freqs[c + 1] * range;
	// Finish the scaling with ceiling division
	return num / total + ((num % total) ? 1 : 0);
}

/*
 * Calculates the lower bound of c, given the restrictions top and bot.
 *
 * If the model has not already been digested, calcLower() digests it 
 * before doing calculations.
 */
uint Model::calcLower(unsigned char c, uint bot, uint top){
	digest();

	// Determine the current range
	int range = top - bot;

	// Scale the bottom of the character onto the current range
	int num = freqs[c] * range;
	// Finish the scaling with ceiling division
	return num / total + ((num % total) ? 1 : 0);
}

/*
 * Calculates the character given an encoding within a certain range.
 *
 * If the model has not already been digested, getChar() digests it before
 * doing calculations.
 */
unsigned char Model::getChar(uint enc, uint bot, uint top){ // TODO: has issues if enc is 0
	digest();

	// Scale enc onto the total number of characters seen
	uint range = top - bot;

	// Binary search freqs for the closest value <= c
	top = 0xFF;
	bot = 0x00;
	uint mid;

	while (top - 1 > bot){
		mid = (top + bot) / 2;
		// std::cout << "(" << bot << ", " << mid << ", " << top << ")\n";
		if (freqs[mid] < enc){
			bot = mid;
		}
		else {
			top = mid;
		}
	}

	return top;	// The character corresponds directly to the index
}

uint Model::getTotal(){
	return total;
}

uint Model::getCharCount(unsigned char c){
	return freqs[c];
}

/*
 * Completely resets the model.
 */
void Model::reset(){
	total = 0;			// Clear total
	digested = false;	// Clear digested
	// Clear each element of freq
	for (int i = 0; i < 256; i++){
		freqs[i] = 0;
	}	
}

/*
 * Writes the current model to an output stream.
 *
 * The model is always undigested afterwards.
 */
void Model::exportModel(std::ostream& out){
	if (digested){
		// Undigesting removes the need to encode intermediate characters
		undigest();
	}

	// First, encode the total
	out.write((char*)&total, sizeof(uint));

	// Encode frequencies other than EOT (0x04)
	int i;
	for(i = 0; i < 4; i++){
		// Only encode frequencies if necessary
		if (freqs[i] > 0){
			out.put((char) i);
			out.write((char*)&freqs[i], sizeof(uint));
		}
	}
	for(i = 5; i < 256; i++){
		// Only encode frequencies if necessary
		if (freqs[i] > 0){
			out.put((char) i);
			out.write((char*)&freqs[i], sizeof(uint));
		}
	}

	// EOT (0x04) gets encoded last
	out.put(0x4);
	out.write((char*)&freqs[4], sizeof(uint));
}

/*
 * Loads a model from an input stream.
 *
 * The model is always undigested afterwards.
 * No error checking.
 */
void Model::importModel(std::istream& in){
	reset();

	in.read((char*)&total, sizeof(uint));

	char c = 0;
	int i = 0;
	// Until after an EOT (0x04) is decoded
	while (c != 4 && in.good()){
		in.get(c);
		in.read((char*)&freqs[(unsigned char)c], sizeof(uint));
	}
}
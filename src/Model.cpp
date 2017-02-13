#include "Model.h"

#include <iostream>

Model::Model(){
	total = 0;
	digested = false;

	for (int i = 0; i < 256; i++){
		freqs[i] = 0;
	}
}

/*
 * Adds a character to the model. 
 *
 * If the model has already been digested, this takes additional
 * time.
 */
bool Model::update(uint8_t c){
	return update(c, 1);
}

bool Model::update(uint8_t c, int count){
	// Prevent exceeding 31 bits of precision
	if (((uint32_t) 0x1 << 31) - 1 - count < total){
		return false;
	}

	// Prevent underflow
	if (count < 0 && freqs[c] + count > freqs[c]){
		return false;
	}

	freqs[c] += count;		// Increment the character's frequency
	total += count;			// Increment the total size

	if (digested){
		// Increment all further entries
		for (int i = c + 1; i < 256; i++){
			freqs[i] += count;
		}
	}

	return true;
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
uint32_t Model::calcUpper(uint8_t c, uint32_t bot, uint32_t top){
	digest();

	// If c is 0, cannot check the previous character's frequency
	uint32_t prev = c ? freqs[c - 1] : 0;
	// If this character has no slots, return the shadow "not present" value
	if (prev == freqs[c]){
		return bot + 1;
	}

	// Find the scaling factor
	// Add 1 for the shadow "not present" value at 0
	uint32_t conversion = (top - bot) / (total + 1);

	// Adding the given bot ensures that it is within the proper range
	// total + 1 due to a shadow "not present" value at 0
	return bot + (freqs[c] + 1) * conversion;

}

/*
 * Calculates the lower bound of c, given the restrictions top and bot.
 *
 * If the model has not already been digested, calcLower() digests it 
 * before doing calculations.
 */
uint32_t Model::calcLower(uint8_t c, uint32_t bot, uint32_t top){
	digest();

	// If c is 0, cannot check the previous character's frequency
	uint32_t prev = c ? freqs[c - 1] : 0;
	// If this character has no slots, return the shadow "not present" value
	if (prev == freqs[c]){
		return bot;
	}

	// Find the scaling factor
	// Add 1 to total for the shadow "not present" value at 0
	uint32_t conversion = (top - bot) / (total + 1); 
	

	// Adding the given bot ensures that it is within the proper range
	// total + 1 due to a shadow "not present" value at 0
	return bot + (prev + 1) * conversion;
}

/*
 * Calculates the character given an encoding within a certain range.
 *
 * If the model has not already been digested, getChar() digests it before
 * doing calculations.
 */
uint8_t Model::getChar(uint32_t enc, uint32_t bot, uint32_t top){
	digest();

	// Scale enc onto the total number of characters seen
	uint32_t conversion = (top - bot) / (total + 1);
	enc = (enc - bot) / conversion;

	// Binary search freqs for the closest value > c
	int upper = 0xFF;	// Inclusive
	int lower = -1; 	// Exclusive
	int mid;

	// A 1 is added to the entries to account for a shadow "not present" value at 0
	while (upper > lower + 1){
		mid = (upper + lower) / 2;
		if (freqs[mid] + 1 > enc){
			upper = mid;
		} else if (freqs[mid] + 1 < enc){
			lower = mid;
		} else{
			lower = mid;
		}
	}

	// The index directly corresponds to the symbol
	return upper;
}

uint32_t Model::getTotal(){
	return total;
}

uint32_t Model::getCharCount(uint8_t c){
	if (digested && c > 0){
		return freqs[c] - freqs[c - 1];
	}
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
		// Undigesting removes the need to encode some symbols
		undigest();
	}

	// First, encode the total
	out.write((char*)&total, sizeof(total));

	// Encode frequencies other than NULL (0)
	int i;
	for (i = 1; i < 256; i++){
		// Only encode frequencies if necessary
		if (freqs[i] > 0){
			out.put((char) i);
			out.write((char*)(freqs + i), sizeof(*freqs));
		}
	}

	// NULL is always encoded (and at the end)
	out.put(0);
	out.write((char*)&freqs[0], sizeof(*freqs));
}

/*
 * Loads a model from an input stream.
 *
 * The model is always undigested afterwards.
 * No error checking.
 */
void Model::importModel(std::istream& in){
	reset();

	in.read((char*)&total, sizeof(total));

	char c = 1;

	// While can read from in and NULL has not been read
	while (c != 0 && in.good()){
		in.get(c);
		in.read((char*)(freqs + (uint8_t)c), sizeof(*freqs));
	}
}
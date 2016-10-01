#include "Model.h"
#include "proto.h"

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
bool Model::update(uint8_t c){
	update(c, 1);
}

bool Model::update(uint8_t c, int count){
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
		// std::cout << i << ": " << freqs[i] << std::endl;
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

	// // Determine the current range
	// uint64_t range = (uint64_t)top - bot + 1;

	// // Scale the top of the character onto the current range
	// uint64_t num = freqs[c] * range;
	// // std::cout << range << ", " << total << ", " << freqs[c] << std::endl;
	// // Keep top inclusive
	// return bot + num / total - ((num % total) ? 0 : 1);

	// TODO: new stuff (see notebook)

	// Determine the current range
	uint32_t range = top - bot;

	// Scale the top of the character onto the current range
	// The freqs are inclusive, but top limit is exclusive, so add a 1
	uint64_t num = ((uint64_t)freqs[c] + 1) * range;

	// Finish the scaling with ceiling division to keep top exclusive
	// Adding the given bot ensures that it is within the proper range
	// total + 1 due to a shadow "not present" value
	return bot + num / (total + 1) + ((num % (total + 1)) ? 1 : 0);

}

/*
 * Calculates the lower bound of c, given the restrictions top and bot.
 *
 * If the model has not already been digested, calcLower() digests it 
 * before doing calculations.
 */
uint32_t Model::calcLower(uint8_t c, uint32_t bot, uint32_t top){
	digest();

	// // Determine the current range
	// uint64_t range = (uint64_t)top - bot + 1;

	// // Scale the bottom of the character onto the current range
	// uint64_t num = range * (c ? freqs[c - 1] : 0);
	// // std::cout << range << ", " << total << ", " << (c ? freqs[c - 1] : 0) << std::endl;
	// // Ceiling division to keep bot inclusive
	// return bot + num / total + ((num % total) ? 1 : 0);

	//TODO: new stuff (see notebook)

	// Determine the current range
	uint32_t range = top - bot;

	// If c is not 0, cannot check the previous character's frequency
	// The previous character's frequency is inclusive, so add 1
	uint64_t prev = c ? freqs[c - 1] + 1 : 1;

	// std::cout << "<Range = " << range << ", prev = " << prev << ">\n";
	// std::cout << "Bot = " << bot << std::endl;

	// Scale the bottom of the character onto the current range
	uint64_t num = prev * range;

	// Finish the scaling with ceiling divison to keep bot inclusive
	// Adding the given bot ensures that it is within the proper range
	// total + 1 due to a shadow "not present" value
	return bot + num / (total + 1) + ((num % (total + 1)) ? 1 : 0); // TODO: change for 0 case
}

/*
 * Calculates the character given an encoding within a certain range.
 *
 * If the model has not already been digested, getChar() digests it before
 * doing calculations.
 */
uint8_t Model::getChar(uint32_t enc, uint32_t bot, uint32_t top){ // TODO: has issues if enc is 0
	digest();
	// std::cout  << "enc: " << enc << std::endl;
	// std::cout << "total: " << total << std::endl;
	// std::cout  << "top: " << top << std::endl;
	// std::cout << "bot: " << bot << std::endl;

	// // Scale enc onto the total number of characters seen
	// uint64_t range = (uint64_t)top - bot + 1;
	// // std::cout << "range: " << range << std::endl;
	// enc = (uint64_t)(enc - bot) * total / range + 1;

	// std::cout  << "enc: " << enc << std::endl;
	// // Binary search freqs for the closest value <= c
	// top = 0xFF;
	// bot = 0x00;
	// uint32_t mid;

	// while (top > bot + 1){
	// 	mid = (top + bot + 1) / 2;
	// 	// std::cout << "(" << bot << ", " << mid << " (" << freqs[mid] << "), " << top << ")\n";
	// 	if (freqs[mid] < enc){
	// 		bot = mid;
	// 	}
	// 	else {
	// 		top = mid;
	// 	}
	// }

	// // std::cout << "Char value: " <<  top << std::endl;
	// return top;	// The character corresponds directly to the index

	//TODO: new stuff (see notebook)

	// Scale enc onto the total number of characters seen
	uint32_t range = top - bot;
	// std::cout << "Enc (before): " << enc << std::endl;
	// Instead of adding 1 to table, subtract 1 from enc to make inclusive
	enc = (uint64_t)(enc - bot) * (total + 1) / range - 1;
	// std::cout << "Enc (after): " << enc + 1 << std::endl;

	// Binary search freqs for the closest value > c
	top = 0xFF;
	bot = 0x00;
	uint32_t mid;

	while (top > bot + 1){
		mid = (top + bot) / 2;
		if (freqs[mid] > enc){
			top = mid;
		} else if (freqs[mid] < enc){
			bot = mid;
		} else{
			bot = mid;
		}
	}

	// The index directly corresponds to the character
	return top;
}

uint32_t Model::getTotal(){
	return total;
}

uint32_t Model::getCharCount(uint8_t c){
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
	out.write((char*)&total, TYPESIZE_BYTES);

	// Encode frequencies other than EOT (0x04)
	int i;
	for(i = 0; i < 4; i++){
		// Only encode frequencies if necessary
		if (freqs[i] > 0){
			out.put((char) i);
			out.write((char*)&freqs[i], TYPESIZE_BYTES);
		}
	}
	for(i = 5; i < 256; i++){
		// Only encode frequencies if necessary
		if (freqs[i] > 0){
			out.put((char) i);
			out.write((char*)&freqs[i], TYPESIZE_BYTES);
		}
	}

	// EOT (0x04) gets encoded last
	out.put(0x4);
	out.write((char*)&freqs[4], TYPESIZE_BYTES);
}

/*
 * Loads a model from an input stream.
 *
 * The model is always undigested afterwards.
 * No error checking.
 */
void Model::importModel(std::istream& in){
	reset();

	in.read((char*)&total, TYPESIZE_BYTES);

	char c = 0;
	int i = 0;
	// Until after an EOT (0x04) is decoded
	while (c != 4 && in.good()){
		in.get(c);
		in.read((char*)&freqs[(uint8_t)c], TYPESIZE_BYTES);
	}
}
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <chrono>
#include <ctime>
#include <cstdlib>

#include "Model.h"
#include "ArEncoder.h"
#include "ArDecoder.h"

uint64_t testUpdateLatency(Model* m, int numTrials, char* randomness);
uint64_t testDigestedUpdateLatency(Model* m, int numTrials, char* randomness);
uint64_t testEncodingLatency(Model* m, int numTrials, char* randomness, std::ostream* ostr);
uint64_t testDecodingLatency(Model* m, int numTrials, char* expected, std::istream* istr);

int main(){
	const int numTrials = 1000000;
	char* randomness = new char[numTrials];

	srand(time(NULL));
	for (int i = 0; i < numTrials; i++){
		randomness[i] = rand() % 256;
	}

	std::stringstream ss;
	Model m;

	uint64_t latency;

	std::cout << "Average latencies:\n";

	latency = testUpdateLatency(&m, numTrials, randomness);
	std::cout << "Update:			" << latency << " ns\n";

	latency = testUpdateLatency(&m, numTrials, randomness);
	std::cout << "Digested update:	" << latency << " ns\n";

	latency = testEncodingLatency(&m, numTrials, randomness, &ss);
	std::cout << "Encoding:		" << latency << " ns\n";

	ss.clear();
	ss.seekg(0);
	latency = testDecodingLatency(&m, numTrials, randomness, &ss);
	std::cout << "Decoding:		" << latency << " ns\n";

	delete[] randomness;
}

uint64_t testUpdateLatency(Model* m, int numTrials, char* randomness){
	uint64_t accum = 0;

	std::chrono::high_resolution_clock::time_point begin;
	std::chrono::high_resolution_clock::time_point end;

	for (int i = 0; i < numTrials; i++){
		begin = std::chrono::high_resolution_clock::now();
		m->update(randomness[i]);
		end = std::chrono::high_resolution_clock::now();

		accum += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	}

	return accum / numTrials;
}

uint64_t testDigestedUpdateLatency(Model* m, int numTrials, char* randomness){
	uint64_t accum = 0;

	std::chrono::high_resolution_clock::time_point begin;
	std::chrono::high_resolution_clock::time_point end;

	m->digest();

	for (int i = 0; i < numTrials; i++){
		begin = std::chrono::high_resolution_clock::now();
		m->update(randomness[i]);
		end = std::chrono::high_resolution_clock::now();

		accum += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	}

	return accum / numTrials;
}

uint64_t testEncodingLatency(Model* m, int numTrials, char* randomness, std::ostream* ostr){
	uint64_t accum = 0;

	std::chrono::high_resolution_clock::time_point begin;
	std::chrono::high_resolution_clock::time_point end;

	ArEncoder are(m, ostr);

	for (int i = 0; i < numTrials; i++){
		begin = std::chrono::high_resolution_clock::now();
		are.put(randomness[i]);
		end = std::chrono::high_resolution_clock::now();

		accum += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	}

	are.finish();

	return accum / numTrials;
}

uint64_t testDecodingLatency(Model* m, int numTrials, char* expected, std::istream* istr){
	uint64_t accum = 0;

	std::chrono::high_resolution_clock::time_point begin;
	std::chrono::high_resolution_clock::time_point end;

	char c;
	bool correct = 1;
	ArDecoder ard(m, istr);

	for (int i = 0; i < numTrials; i++){
		begin = std::chrono::high_resolution_clock::now();
		c = ard.get();
		end = std::chrono::high_resolution_clock::now();
		if (c != expected[i]){
			std::cout << "Incorrect reading at position " << i << std::endl;
			std::cout << "Expected	" << (int) expected[i] << std::endl;
			std::cout << "Got		" << (int) c << std::endl;
			correct = 0;
		}

		accum += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	}

	if (!correct){
		std::cout << "Incorrect decoding\n";
	}
	return accum / numTrials;
}
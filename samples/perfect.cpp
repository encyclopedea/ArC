#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Model.h"
#include "ArEncoder.h"
#include "ArDecoder.h"

void printHelpMsg();
int checkHeader(std::istream& ifs);
void putHeader(std::ofstream& ofs);
int decode(std::string inputFile, std::string outputFile);
int encode(std::string inputFile, std::string outputFile);

const std::string header = "perfect_sample";

int main(int argc, char** argv){
	if (argc < 4){
		printHelpMsg();
		return 0;
	}

	int e = 0;
	int d = 0;

	int opt;
	opterr = 0;
	while ((opt = getopt(argc, argv, "edh")) != -1){
		switch(opt){
			case 'e':
				e = 1;
				break;
			case 'd':
				d = 1;
				break;
			case 'h':
				printHelpMsg();
				return 0;
			case '?':
				std::cout << "Unknown options '-" << optopt << "'.\n";
				printHelpMsg();
				return 1;
			default:
				std::cout << "An unknown error occurred\n";
				printHelpMsg();
				return 1;
		}
	}

	if (e && d){
		std::cout << "\nOnly one of -e and -d may be specified.\n";
	} else if (e){
		encode(argv[optind], argv[optind + 1]);
	} else if (d){
		decode(argv[optind], argv[optind + 1]);
	} else{
		std::cout << "\nExactly one of -d and -e should be specified.\n";
	}

	return 0;
}

void printHelpMsg(){
	std::cout << "Usage: perfect_sample <input file> <output file> -opts\n";
	std::cout << "Options:";
	std::cout << "\n	-e	encode";
	std::cout << "\n	-d	decode";
	std::cout << "\nExactly one of -d and -e should be specified.\n";
}

int encode(std::string inputFile, std::string outputFile){
	std::ifstream ifs(inputFile.c_str());
	std::ofstream ofs(outputFile.c_str());

	if (!ifs.good()){
		std::cout << "Error opening file for input.\n";
		return 1;
	}

	if (!ofs.good()){
		std::cout << "Error opening file for output.\n";
		return 1;
	}

	putHeader(ofs);

	// USAGE OF LIBRARY
	Model m;
	ArEncoder are(&m, &ofs);

	// Initialize the model to be a perfect representation of the file
	char c = ifs.get();
	while (ifs.good()){
		m.update(c);
		c = ifs.get();
	}
	ifs.clear();
	ifs.seekg(0);

	m.exportModel(ofs);

	// Start encoding the characters
	int i = 0;
	c = ifs.get();
	while (ifs.good()){
		i++;
		are.put(c);
		m.update(c, -1);	// We already used this instance of the character, won't be seeing it again
		c = ifs.get();
	}

	are.finish();

	// END USAGE OF LIBRARY

	std::cout << "Encoded " << i << " characters.\n";

	return 0;
}

int decode(std::string inputFile, std::string outputFile){
	std::ifstream ifs(inputFile.c_str());
	std::ofstream ofs(outputFile.c_str());

	if (!ifs.good()){
		std::cout << "Error opening file for input.\n";
		return 1;
	}

	if (!ofs.good()){
		std::cout << "Error opening file for output.\n";
		return 1;
	}

	if (!checkHeader(ifs)){
		std::cout << "The header does not match. Please verify that this file is in the correct format.\n";
		return 1;
	}

	// USAGE OF LIBRARY
	Model m;

	m.importModel(ifs);
	ArDecoder ard(&m, &ifs);	// Constructor reads from input, so create AFTER importing model
	
	unsigned int i = 0;
	unsigned int total = m.getTotal();	// Will be changing m.getTotal later
	char c;
	c = ard.get();
	while (i < total){ // While there are still characters to read
		m.update(c, -1);
		i++;
		ofs.put(c);
		c = ard.get();
	}
	// END USAGE OF LIBRARY

	std::cout << "Decoded " << i << " characters.\n";

	return 0;
}

void putHeader(std::ofstream& ofs){
	ofs.write(header.c_str(), header.length());
}

int checkHeader(std::istream& ifs){
	char* buf = new char[header.length() + 1];
	ifs.read(buf, header.length());
	buf[header.length()] = '\0'; // Null terminate

	int ret = (header == std::string(buf));
	delete[] buf;
	return ret;
}
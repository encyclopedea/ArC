#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Model.h"
#include "ArEncoder.h"
#include "ArDecoder.h"
//TODO: finish, copy, then do basic

void printHelpMsg();
int checkHeader(std::istream& ifs);
void putHeader(std::ofstream& ofs);
int decode(std::string inputFile, std::string outputFile);
int encode(std::string inputFile, std::string outputFile);

const std::string header = "adaptive_sample";

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
	std::cout << "Usage: adaptive_sample <input file> <output file> -opts\n";
	std::cout << "Options:";
	std::cout << "\n\t-e\tencode";
	std::cout << "\n\t-d\tdecode";
	std::cout << "\nExactly one of -d and -e should be specified.\n";
}

int encode(std::string inputFile, std::string outputFile){
	std::ifstream ifs(inputFile.c_str());
	std::ofstream ofs(outputFile.c_str());

	if (!(ifs.good() && ofs.good())){
		std::cout << "Error opening files for input/output.\n";
		return 1;
	}

	putHeader(ofs);

	// USAGE OF LIBRARY
	Model m;
	ArEncoder are(&m, &ofs);

	// Initialize m with every character having a slot
	for (int i = 0; i < 256; i++){
		m.update(i);
	}

	char c = ifs.get();
	while (ifs.good()){
		are.put(c);
		m.update(c);
		c = ifs.get();
	}

	// Terminate with an EOT (0x4)
	are.put(0x4);

	are.finish();

	// END USAGE OF LIBRARY

	return 0;
}

int decode(std::string inputFile, std::string outputFile){
	std::ifstream ifs(inputFile.c_str());
	std::ofstream ofs(outputFile.c_str());

	if (!(ifs.good() && ofs.good())){
		std::cout << "Error opening files for input/output.\n";
		return 1;
	}

	if (!checkHeader(ifs)){
		std::cout << "The header does not match. Are you sure this file is in the correct format?\n";
		return 1;
	}

	// USAGE OF LIBRARY
	Model m;
	ArDecoder ard(&m, &ifs);

	// Initialize m with every character having a slot
	for (int i = 0; i < 256; i++){
		m.update(i);
	}

	char c;
	c = ard.get();
	while (c != 0x04){ // While an EOT has not been read
		m.update(c);
		ofs.put(c);
		c = ard.get();
	}

	// END USAGE OF LIBRARY

	return 0;
}

void putHeader(std::ofstream& ofs){
	ofs.write(header.c_str(), header.length());
}

int checkHeader(std::istream& ifs){
	char buf[header.length() + 1];
	ifs.read(buf, header.length());
	buf[header.length()] = '\0'; // Null terminate

	return header == std::string(buf);
}
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Model.h"
#include "ArEncoder.h"
#include "ArDecoder.h"

void printHelpMsg();
int checkHeader(std::istream& ifs);
void putHeader(std::ofstream& ofs);
void initModel(Model* m);
int decode(std::string inputFile, std::string outputFile);
int encode(std::string inputFile, std::string outputFile);

const std::string header = "heuristic_sample";

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
	std::cout << "Usage: heuristic_sample <input file> <output file> -opts\n";
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
	initModel(&m);

	int i = 0;
	char c = ifs.get();
	while (ifs.good()){
		i++;
		are.put(c);
		c = ifs.get();
	}
	std::cout << "Encoded " << i << " characters.\n";

	// Terminate with an EOT (0x4)
	are.put(0x4);

	are.finish();

	// END USAGE OF LIBRARY

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
	ArDecoder ard(&m, &ifs);
	initModel(&m);

	int i = 0;
	char c;
	c = ard.get();
	while (c != 0x04){ // While an EOT has not been read
		ofs.put(c);
		c = ard.get();
		i++;
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
	buf[header.length()] = '\0';

	int ret = (header == std::string(buf));
	delete[] buf;
	return ret;
}

void initModel(Model* m){
	// USAGE OF LIBRARY

	// Collected from the Complete Works of William Shakespeare
	// as found at http://www.gutenberg.org/cache/epub/100/pg100.txt
	m->update('\n', 124456);
	m->update(' ', 1293934);
	m->update('!', 8844);
	m->update('"', 470);
	m->update('#', 1);
	m->update('%', 1);
	m->update('&', 21);
	m->update('\'', 31069);
	m->update('(', 628);
	m->update(')', 629);
	m->update('*', 63);
	m->update(',', 83174);
	m->update('-', 8074);
	m->update('.', 78025);
	m->update('/', 5);
	m->update('0', 299);
	m->update('1', 928);
	m->update('2', 366);
	m->update('3', 330);
	m->update('4', 93);
	m->update('5', 82);
	m->update('6', 63);
	m->update('7', 41);
	m->update('8', 40);
	m->update('9', 948);
	m->update(':', 1827);
	m->update(';', 17199);
	m->update('<', 468);
	m->update('=', 1);
	m->update('>', 441);
	m->update('?', 10476);
	m->update('@', 8);
	m->update('A', 44486);
	m->update('B', 15413);
	m->update('C', 21497);
	m->update('D', 15683);
	m->update('E', 42583);
	m->update('F', 11713);
	m->update('G', 11164);
	m->update('H', 18462);
	m->update('I', 55806);
	m->update('J', 2067);
	m->update('K', 6196);
	m->update('L', 23858);
	m->update('M', 15872);
	m->update('N', 27338);
	m->update('O', 33209);
	m->update('P', 11939);
	m->update('Q', 1178);
	m->update('R', 28970);
	m->update('S', 34011);
	m->update('T', 39800);
	m->update('U', 14129);
	m->update('V', 3580);
	m->update('W', 16496);
	m->update('X', 606);
	m->update('Y', 9099);
	m->update('Z', 532);
	m->update('[', 2085);
	m->update(']', 2077);
	m->update('_', 71);
	m->update('`', 1);
	m->update('a', 244664);
	m->update('b', 46543);
	m->update('c', 66688);
	m->update('d', 133779);
	m->update('e', 404621);
	m->update('f', 68803);
	m->update('g', 57035);
	m->update('h', 218406);
	m->update('i', 198184);
	m->update('j', 2712);
	m->update('k', 29212);
	m->update('l', 146161);
	m->update('m', 95580);
	m->update('n', 215924);
	m->update('o', 281391);
	m->update('p', 46525);
	m->update('q', 2404);
	m->update('r', 208894);
	m->update('s', 214978);
	m->update('t', 289975);
	m->update('u', 114818);
	m->update('v', 33989);
	m->update('w', 72894);
	m->update('x', 4688);
	m->update('y', 85271);
	m->update('z', 1099);
	m->update('|', 33);
	m->update('}', 2);
	m->update('~', 1);

	m->update(0x4, 1);	// For termination

	// END USAGE OF LIBRARY
}
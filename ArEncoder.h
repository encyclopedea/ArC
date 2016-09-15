#include <ostream>

#include "Model.h"

class ArEncoder{
public:
	ArEncoder(Model* m, std::ostream* out);
	~ArEncoder();

	bool put(unsigned char c);
	int finish();
private:
	Model* m;
	std::ostream* out;
	unsigned char buf;
	int bufcurs;
	int pending;
	unsigned int top;
	unsigned int bot;

	bool outputBit(char c);
	int outputPending(char c);
};
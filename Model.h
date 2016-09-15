#include <iosfwd>

typedef unsigned int uint;

class Bitstream;

class Model{
public:
	Model();
	~Model(){}

	bool update(unsigned char c);
	bool update(unsigned char c, int count);
	void digest();

	uint calcUpper(unsigned char c, uint bot, uint top);
	uint calcLower(unsigned char c, uint bot, uint top);

	unsigned char getChar(uint enc, uint bot, uint top);
	uint getTotal();
	uint getCharCount(unsigned char c);

	void reset();

	void exportModel(std::ostream& out);
	void importModel(std::istream& in);

private:
	uint freqs[256]; // 1 more than size of char; the last value is not used
	uint total = 0;
	bool digested = false;

	void undigest();
};
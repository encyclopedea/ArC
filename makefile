# test: test.cpp lib/libArC.a
# 	g++ -o test test.cpp -L ./lib -lArC

# filetest: filetest.cpp lib/libArC.a
# 	g++ -o filetest filetest.cpp -L ./lib -lArC


CPP 	:= g++
OBJECTS := ArEncoder.o ArDecoder.o Model.o
LIBS	:= -L lib -lArC
INCLUDES:= -I src

# General

.PHONY: all
all: lib/libArC.a samples

.PHONY: samples
samples: $(patsubst samples/%.cpp,%_sample,$(wildcard samples/*))

%_sample: samples/%.cpp lib/libArC.a
	$(CPP) -o $*_sample samples/$*.cpp $(LIBS) $(INCLUDES)

# The core

lib/libArC.a: $(OBJECTS)
	mkdir -p lib
	ar rcs lib/libArC.a $(OBJECTS)

# Object files

ArEncoder.o: src/ArEncoder.cpp src/ArEncoder.h
	$(CPP) -c src/ArEncoder.cpp

ArDecoder.o: src/ArDecoder.cpp src/ArDecoder.h
	$(CPP) -c src/ArDecoder.cpp

Model.o: src/Model.cpp src/Model.h
	$(CPP) -c src/Model.cpp

# Clean

.PHONY: clean
clean:
	rm -rf *.o lib *_sample

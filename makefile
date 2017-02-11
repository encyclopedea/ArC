CPP 	:= g++
OBJECTS := ArEncoder.o ArDecoder.o Model.o
LIBS	:= -L lib -lArC
INCLUDES:= -I src
FLAGS	:= -O3 -Wall -Wextra -Wpedantic -Wshadow


# General

.PHONY: all
all: lib/libArC.a samples

.PHONY: samples
samples: $(patsubst samples/%.cpp,%_sample,$(wildcard samples/*))

%_sample: samples/%.cpp lib/libArC.a
	$(CPP) -o $*_sample samples/$*.cpp $(LIBS) $(INCLUDES) $(FLAGS)


# The library

lib/libArC.a: $(OBJECTS)
	mkdir -p lib
	ar rcs lib/libArC.a $(OBJECTS)


# Object files

ArEncoder.o: src/ArEncoder.cpp src/ArEncoder.h
	$(CPP) -c src/ArEncoder.cpp $(FLAGS)

ArDecoder.o: src/ArDecoder.cpp src/ArDecoder.h
	$(CPP) -c src/ArDecoder.cpp $(FLAGS)

Model.o: src/Model.cpp src/Model.h
	$(CPP) -c src/Model.cpp $(FLAGS)


# Clean

.PHONY: clean
clean:
	rm -rf *.o lib *_sample

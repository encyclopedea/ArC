This Arithmetic Coding library is meant to expand my understanding of current compression techniques and practice my coding.

##Overview
* Model is the core to this library. It stores frequencies, calculates bounds, and looks up characters in an internal table.
* ArEncoder is the encoder. It uses a Model (which it does not modify or export) and an istream to encode characters and output bits as necessary.
* ArDecoder is the decoder. It uses a Model (which it does not modify or import) and an ostream to decode characters.

##Usage
* Compiler flags: `-L path/to/ArC/lib -lArC -I path/to/ArC/src`
* Includes: ArEncoder.h, ArDecoder.h, Model.h

##Usage Notes and Suggestions
* This code is meant to have lots of flexibility by being less structured.
* Model
  * A Model can be freely manipulated, even while in use. 
    * This means that an adaptive coding stream can be achieved by encoding/decoding characters then updating the Model.
    * Alternatively, the frequency counts can be simplified, reduced, expanded, or otherwise changed to fit within the limitations.
  * A Model can be reused by use of its reset() method.
  * Models are not automatically imported or exported by any other class. It is up to the developer to import or export Models.
  * NULL always has at least one slot, since it is used for encoding symbols with frequencies of 0. This cannot be changed by calling update().
* ArEncoder
  * ArEncoder must call finish() when done encoding, or up to 39 bits will remain in its internal buffers without being output, resulting in lost characters.
* ArDecoder
  * ArDecoder begins reading from the input stream on construction.
  * ArDecoder does not know when to stop. It is up to the developer to decide a stopping condition and stop decoding characters.
    * Note that even when the error flags are set, valid characters may remain encoded. For this reason, ArDecoder can continue decoding characters even while it cannot read more characters from the input stream. 

##Samples
* To make all samples: `make samples`
* To make a specific sample: `make <samplename>_sample`
* All samples compile to an executable named "<samplename>_sample"
* All code in samples that directly uses ArC is wrapped in "USAGE OF LIBRARY" and "END USAGE OF LIBRARY" comments
* List of current samples:
  * adaptive
    * Demonstrates an adaptive style of coding where the model is updated after every character encoded/decoded. Use `./adaptive_sample -h` for usage. 

##Limitations
* The total number of values (eg the number of characters injested) in Model cannot exceed 2 ^ 31
  * Any more will result in undefined behavior.
  * Therefore, it is suggested that for very large inputs, the frequency table in Model be simplified every so often.
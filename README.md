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
  * ArEncoders should not be reused.
* ArDecoder
  * ArDecoder begins reading from the input stream on construction.
  * ArDecoder does not know when to stop. It is up to the developer to decide a stopping condition and stop decoding characters.
    * Note that even when the error flags are set, valid characters may remain encoded. For this reason, ArDecoder can continue decoding characters even while it cannot read more characters from the input stream. 
  * ArDecoders should not be reused.

##Documentation
Note: This documentation includes only the functions that are intended for use by the user of this library. Other functions are publically available, but are intended for internal use.

###Model

| Function | Arguments | Role | Returns |
|----------|-----------| -----|---------|
| Model    | None | Constructor | N/A |
| update   | **(uint8_t) c** The character to be updated | Increments the internal count of a character by 1. If the model has already been digested, this takes additional time. If the update would violate the 31 bit precision limits, it does not occur and returns false. | **(bool)** Returns false if the update failed, true otherwise. |
| update   | **(uint8_t) c** The character to be updated <br/><br/>**(int) count** The amount to update by | Increments (or, if count is negative, decrements) the internal count of a character by a specified amount. If the model has already been digested, this takes additional time. If the update would violate the 31 bit precision limits or, in the case of a negative **count**, would underflow **c**'s interal count, it does not occur and returns false. | **(bool)** Returns false if the update failed, true otherwise. |
| digest   | None | Digests the current model. Digestion is required for most of the other member functions to operate (many of them will call digest() if it has not occurred before proceeding). After digestion, both update() overloads take additional time. | void |
| getTotal | None | Provides access to the total number of characters ingested. Care should be taken to avoid exceeding the limits (see Limitations). | **(uint32_t)** The total number of characters ingested.|
| getCharCount | **(uint8_t) c** The character to check | Provides access to individual character counts. | **(uint32_t)** The internal count of ther specified character |
| reset | None | Resets the Model. | void |
| exportModel | **(std::ostream&) out** The stream to which the Model state will be output | Writes the current state of the Model to a stream (often a file). | void |
| importModel | **(std::istream&) in** The stream from which the Model state will be read | Loads a Model state from an input (often a file), which overwrites the current Model state. | void |

###ArEncoder
| Function | Arguments | Role | Returns |
|----------|-----------| -----|---------|
| ArEncoder | **(Model\*) m** A pointer to the Model to be used <br/><br/> **(std::ostream\*) out** A point to the output stream | Constructor | N/A |
| put | **(uint8_t) c** The character to be encoded | Encodes a single character and outputs bits to the output stream as necessary. | **(bool)** False if the Model or outputstream are NULL. Otherwise, true. |
| finish | None | Writes the remaining bits in the internal buffers to the output stream. This should be called after every full encoding, at the risk of losing characters. This is NOT called by the destructor. | **(int)** If out is NULL, -1. Otherwise, this is the number of bits that were output from the internal buffers. |

###ArDecoder
| Function | Arguments | Role | Returns |
|----------|-----------| -----|---------|
| ArDecoder | **(Model\*) m** A pointer to the Model to be used <br/><br/> **(std::istream\*) in** A pointer to the input stream | Constructor | N/A |
| get | None | If no error flags are set, or need to be set, then this decodes a single character. | **(uint8_t)** The decoded character, or, if an error occurred, 0 |
| good | None | Tells the state of the stream. | **(bool)** Returns false if no error flags are set, and true otherwise. |


##Samples
* To make all samples: `make samples`
* To make a specific sample: `make <samplename>_sample`
* All samples compile to an executable named "<samplename>_sample"
* All code in samples that directly uses ArC is wrapped in "USAGE OF LIBRARY" and "END USAGE OF LIBRARY" comments
* List of current samples:
  * adaptive
    * Demonstrates an adaptive style of coding where the model is updated after every character encoded/decoded. Use `./adaptive_sample -h` for usage information. 
  * heuristic
    * Demonstrates the use of a static model based on a heuristic (in this case, the frequency counts of each character in the complete works of William Shakespeare, as found at http://www.gutenberg.org/cache/epub/100/pg100.txt). Use `./heuristic_sample -h` for usage information.
  * benchmark
    * Measures the latency for several important operations over averaged over 1000000 trials. To use: `./benchmark_sample`.

##Limitations
* There is a 31 bit precision limit due to the use of 32 bit values during the encoding.
  * The total number of values (eg the number of characters ingested) in Model cannot exceed 2 ^ 31 - 1
  * Any more will result in undefined behavior.
  * Therefore, it is suggested that for very large inputs, the frequency table in Model be simplified every so often.

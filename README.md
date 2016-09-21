This Arithmetic Coding library is meant to expand my understanding of current compression techniques and practice my coding.


Limitations:
* The total number of values (eg the number of characters injested) in Model cannot exceed 2 ^ 31
  * Any more will result in undefined behavior.
  * This corresponds to half a gigabyte's worth of characters.
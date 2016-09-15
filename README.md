Limitations:
* The total number of values (eg the number of characters injested) in Model cannot exceed 2 ^ (sizeof(int) * 8 - 1)
  * Any more will result in undefined behavior.
  * On a system that uses 32 bit ints, this corresponds to half a gigabyte worth of characters.
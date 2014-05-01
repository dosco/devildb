#ifndef INCLUDE_BLOOM_FILTER_H
#define INCLUDE_BLOOM_FILTER_H

#include <string>
#include <vector>
//#include <iostream>
#include <fstream>
#include <cmath>
//#include <boost/dynamic_bitset.hpp>

#include "murmur3_hash.h"
#include "math.h"

class BloomFilter
{
private:
	int num_of_hashes;
	//boost::dynamic_bitset<> bits;
	//std::vector<bool> bits;
	char *bits;
	size_t byte_array_size;
	size_t bit_array_size;

public:
  BloomFilter() { }

  BloomFilter(std::ifstream &is) {
    deserialize(is);
  }

	BloomFilter(int capacity, double false_positive_probability) {
		// SIze of bit array base of the capacity and false_positive_probability
		bit_array_size = (int)(- (double)(capacity) * log(false_positive_probability)/ ( log(2.0) * log(2.0) ));

		// Total num of hashes for the false_positive_probability (research work
		num_of_hashes = (int)((double) bit_array_size * log(2.0) / capacity);

		// Init bits all with values false
		//bits = std::vector<bool>(bit_array_size, false);
		//bits(bit_array_size);
		byte_array_size = (int) ceil((bit_array_size + 7) / 8);
		bits = new char[byte_array_size];
	}

	~BloomFilter() {
	  delete[] bits;
	}

	void add(std::string item) {
    unsigned int result = 0;
    // Hash each item into and int, and set the corresponding bit
    // Moding if its over the limit
    for(int i=0; i<num_of_hashes; i++) {
		  // MurmurHash3_x86_32 will return an int representing the hash in result
		  MurmurHash3_x86_32(item.c_str(), item.length(), i, &result);
		  //bits[result%bits.size()] = 1;
		  int b = result % bit_array_size;
		  int n = (int) b / 8;
		  bits[n] |= 1 << (b % 8);
	  }
	}

  void add(const char *data, size_t len) {
    unsigned int result = 0;
    // Hash each item into and int, and set the corresponding bit
    // Moding if its over the limit
    for(int i=0; i<num_of_hashes; i++) {
      // MurmurHash3_x86_32 will return an int representing the hash in result
      MurmurHash3_x86_32(data, len, i, &result);
      //bits[result%bits.size()] = 1;
      int b = result % bit_array_size;
      int n = (int) floor(b / 8);
      bits[n] |= 1 << (b - n);
    }
  }

	bool contains(std::string item) {
    unsigned int result = 0;
    for(int i=0; i<num_of_hashes; i++)
    {
      MurmurHash3_x86_32(item.c_str(), item.length(), i, &result);
      // For an item to be there, all the bits corresponding to the hashes have to be set
      int b = result % bit_array_size;
      int n = (int) b / 8;
      if((bits[n] & (1 << (b % 8))) == 0) {
      //if(bits[result%bits.size()] == 0) {
        return false;
      }
	  }

	  return true;
	}

	void serialize(std::ofstream &os) {
    os.write((const char *) &num_of_hashes, sizeof(int));
    os.write((const char *) &bit_array_size, sizeof(size_t));
    os.write((const char *) &byte_array_size, sizeof(size_t));
    os.write((const char *) bits, byte_array_size);
	}

	void deserialize(std::ifstream &is) {
    is.read((char *) &num_of_hashes, sizeof(int));
    is.read((char *) &bit_array_size, sizeof(size_t));
    is.read((char *) &byte_array_size, sizeof(size_t));

    bits = new char[byte_array_size];
    is.read((char *) bits, byte_array_size);
  }

};

#endif

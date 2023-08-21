#include <iostream>
#include <chrono>
#include <ctime>
#include "sha256.h"

/*
https://github.com/System-Glitch/SHA256

$ ./SHA256 "string" "string2"
473287f8298dba7163a897908958f7c0eae733e25d2e027992ea2edc9bed2fa8
b993212a26658c9077096b804cdfb92ad21cf1e199e272c44eb028e45d07b6e0
*/

int main(int argc, char ** argv) {

uint8_t data [] = {0x0c, 0x7d, 0x32, 0xd4, 0xb9, 0x98, 0xa1, 0xb8, 0x44, 0x32, 0xc7, 0x88, 0x80, 0x22, 0x66, 0xa1,
0x88, 0x2e, 0x5b, 0x28, 0xc4, 0x87, 0x32, 0x0d, 0x09, 0x9e, 0xc6, 0x4c, 0x1b, 0x3a, 0x46, 0x87,
0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
0x07, 0x08, 0x09, 0x00, 0x16, 0x00, 0x00}; 

		SHA256 sha;
		sha.update(data, sizeof(data));
		uint8_t * digest = sha.digest();

		std::cout << SHA256::toString(digest) << std::endl;

		delete[] digest;
	

	return EXIT_SUCCESS;
}

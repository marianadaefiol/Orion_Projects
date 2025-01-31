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

	for(int i = 1 ; i < argc ; i++) {
		SHA256 sha;
		sha.update(argv[i]);
		uint8_t * digest = sha.digest();

		std::cout << SHA256::toString(digest) << std::endl;

		delete[] digest;
	}

	return EXIT_SUCCESS;
}

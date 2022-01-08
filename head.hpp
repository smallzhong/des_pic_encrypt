#pragma once
#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <time.h>

using namespace std;

bitset<64> encrypt_ECB(bitset<64> plain);
bitset<64> decrypt_ECB(bitset<64> cipher);
bitset<64> decrypt_CBC(bitset<64>& cipher, bitset<64> iv);
bitset<64> encrypt_CBC(bitset<64>& cipher, bitset<64> iv);
bitset<64> decrypt_CFB(bitset<64>& cipher, bitset<64> iv);
bitset<64> encrypt_CFB(bitset<64>& cipher, bitset<64> iv);
bitset<64> charToBitset(const char s[8]);
void init_des(string k);
void init_des_bitset(bitset<64> k);

extern "C" int getopt(int argc, char* const* argv, const char* optstring);

typedef union _LARGE_INTEGER {
    struct {
        unsigned int LowPart;
        int HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        unsigned int LowPart;
        int HighPart;
    } u;
    long long QuadPart;
    bitset<64> bits;
} LARGE_INTEGER;

#define EXIT_ERROR(x)                                 \
	do                                                \
	{                                                 \
		cout << "error in file " << __FILE__ << " in line " << __LINE__ << endl; \
		cout << x;                                    \
		getchar();                                    \
		exit(EXIT_FAILURE);                           \
	} while (0)

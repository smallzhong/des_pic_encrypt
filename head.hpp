#pragma once
#include <iostream>
#include <fstream>
#include <bitset>
#include <string>

using namespace std;

bitset<64> encrypt_ECB(bitset<64>& plain);
bitset<64> decrypt_ECB(bitset<64>& cipher);
bitset<64> decrypt_CBC(bitset<64>& cipher, bitset<64> iv);
bitset<64> encrypt_CBC(bitset<64>& cipher, bitset<64> iv);
bitset<64> charToBitset(const char s[8]);

void init_des(string k);
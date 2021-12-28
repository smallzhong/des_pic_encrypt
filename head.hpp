#pragma once
#include <iostream>
#include <fstream>
#include <bitset>
#include <string>

using namespace std;

bitset<64> encrypt_ECB(bitset<64>& plain);
bitset<64> decrypt_ECB(bitset<64>& cipher);
void init_des(string k);
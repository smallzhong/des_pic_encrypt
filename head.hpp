#pragma once
#include <iostream>
#include <fstream>
#include <bitset>
#include <string>

using namespace std;

bitset<64> encrypt(bitset<64>& plain);
bitset<64> decrypt(bitset<64>& cipher);
void init_des(string k);
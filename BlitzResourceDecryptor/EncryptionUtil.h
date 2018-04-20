#pragma once

#include <cstdint>

int DecryptAes128Cbc(uint8_t* sourceBuffer, uint8_t* destBuffer, int sourceSize, uint8_t* key, uint8_t* iv);
int GenerateAes128Cmac(uint8_t* sourceBuffer, uint8_t* destBuffer, int sourceSize, uint8_t* key);
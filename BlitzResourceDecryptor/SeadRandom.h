#pragma once

#include <cstdint>

struct SeadRandom
{
public:
	SeadRandom(uint32_t seed);
	uint32_t getU32();
private:
	uint8_t internalData[16];
};

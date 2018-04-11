#pragma once

#include <cstdint>

struct SeadRandom
{
public:
	SeadRandom(uint32_t seed);
	SeadRandom(unsigned int seedOne, unsigned int seedTwo, unsigned int seedThree, unsigned int seedFour);
	uint32_t getU32();
private:
	uint8_t internalData[16];
};

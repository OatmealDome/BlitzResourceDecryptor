#include "stdafx.h"

#include "SeadRandom.h"

#define _DWORD unsigned int

SeadRandom::SeadRandom(uint32_t seed)
{
	unsigned int v2; // w8
	unsigned int v3; // w10
	unsigned int v4; // w11

	v2 = 1812433253 * (seed ^ (seed >> 30)) + 1;
	v3 = 1812433253 * (v2 ^ (v2 >> 30)) + 2;
	v4 = 1812433253 * (v3 ^ (v3 >> 30)) + 3;
	*(_DWORD *)internalData = v2;
	*(_DWORD *)(internalData + 4) = v3;
	*(_DWORD *)(internalData + 8) = v4;
	*(_DWORD *)(internalData + 12) = 1812433253 * (v4 ^ (v4 >> 30)) + 4;
}

SeadRandom::SeadRandom(unsigned int seedOne, unsigned int seedTwo, unsigned int seedThree, unsigned int seedFour)
{
	if (seedOne | seedTwo | seedThree | seedFour) {
		*(_DWORD *)internalData = seedOne;
		*(_DWORD *)(internalData + 4) = seedTwo;
		*(_DWORD *)(internalData + 8) = seedThree;
		*(_DWORD *)(internalData + 12) = seedFour;
	}
	else
	{
		*(_DWORD *)internalData = 0x00000001;
		*(_DWORD *)(internalData + 4) = 0x6C078967;
		*(_DWORD *)(internalData + 8) = 0x714ACB41;
		*(_DWORD *)(internalData + 12) = 0x48077044;
	}
}

uint32_t SeadRandom::getU32()
{
	unsigned int v1;
	unsigned int v2;
	unsigned int v3;

	v1 = *(_DWORD*)internalData ^ (*(_DWORD*)internalData << 11);
	*(_DWORD*)internalData = *((_DWORD*)internalData + 1);
	v2 = *((_DWORD*)internalData + 3);
	v3 = v1 ^ (v1 >> 8) ^ v2 ^ (v2 >> 19);
	*((_DWORD*)internalData + 1) = *((_DWORD*)internalData + 2);
	*((_DWORD*)internalData + 2) = v2;
	*((_DWORD*)internalData + 3) = v3;

	return v3;
}

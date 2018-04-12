// BlitzResourceDecryptor.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "CRC.h"
#include "EncryptionUtil.h"
#include "SeadRandom.h"

#define _WORD unsigned short
#define _DWORD unsigned int

char lookupString[] = "e413645fa69cafe34a76192843e48cbd691d1f9fba87e8a23d40e02ce13b0d534d10301576f31bc70b763a60cf07149cfca50e2a6b3955b98f26ca84a5844a8aeca7318f8d7dba406af4e45c4806fa4d7b736d51cceaaf0e96f657bb3a8af9b175d51b9bddc1ed475677260f33c41ddbc1ee30b46c4df1b24a25cf7cb6019794";

char* magicNumbers = "nisasyst";

void CreateByteSequence(SeadRandom* random, uint8_t* outputBuffer)
{
	unsigned __int64 v10 = 0;
	unsigned int v11;
	unsigned int v12;
	char v13;
	char v14;
	char v28[5];

	int counter;

	do
	{
		v11 = random->getU32() >> 24;
		v12 = random->getU32() >> 24;
		v13 = lookupString[v11]; // get digit one
		v14 = lookupString[v12]; // get digit two

		*(_DWORD*)v28 = 808482864; // "0x" (last two characters overwritten after this)
		v28[2] = v13; // copy first digit
		*(_WORD*)&v28[3] = v14; // copy second digit

		// resulting str = 0x##

		counter = v10 + 1;
		outputBuffer[v10] = (uint8_t)std::strtoul(v28, 0, 16); // to uint8_t

		v10 = v10 & 0xFFFFFFFF00000000LL | (v10 + 1) & 0xFFFFFFFF;
	} while (counter != 16);
}

void CreateSaveKey(uint8_t* buffer, SeadRandom* random)
{
	const unsigned int lookupTable[] = { 0x476BC5E7, 0x76DF4A26, 0x2BD714E2, 0x632623A5, 0xD36838ED,
		0x7583042F, 0x32B3A6CE, 0x301180A2, 0x63479874, 0xB0EEF148,
		0x4144F37F, 0x717EDD13, 0xA7C02D5F, 0xDF535C64, 0x1816AA04,
		0xC3E911A8, 0x7E6D14A, 0xFAE665ED, 0xE0A4AA9E, 0xB271F407,
		0xE57CCA4F, 0xD2CBFF86, 0x33AC5C1A, 0x10867E0A, 0x42E43493,
		0x8CCBC746, 0x1E82ECBC, 0xFDE23A9C, 0x52A93E3D, 0x617A7898,
		0x753255C, 0xDFF1DEC9, 0x310E0F37, 0x1C4BA740, 0x8E02AB06,
		0xDDBE8580, 0xC714785E, 0xB24EB7B, 0xE6951F2F, 0x728EBF81,
		0xA10ABBBF, 0x47F5244F, 0xF96AD9DA, 0x8B8472CD, 0xEE47B55B,
		0xA1E97980, 0xF30B7FDA, 0xFD230EE1, 0x7BF84A0E, 0x705A2AFC,
		0x6685E6A1, 0x98AAB220, 0x2B307047, 0x551804EB, 0x183A95BB,
		0x4531F3E8, 0xFDCB1756, 0xF0387032, 0x1F27AC7D, 0x5AD014E2,
		0x6508E3B3, 0xF13D7C92, 0xD7DA45D4, 0xA01D9485 };

	uint8_t* v3 = buffer;
	unsigned int v5, v6, v7, v8;

	__int64 v4 = 0LL;
	do
	{
		*(_DWORD *)(v3 + v4) = 0;
		v5 = lookupTable[(unsigned int)random->getU32() >> 26];
		*(_DWORD *)(v3 + v4) = ((v5 >> (((unsigned int)random->getU32() >> 27) & 0x18)) & 0xFF | *(_DWORD *)(v3 + v4)) << 8;
		v6 = lookupTable[(unsigned int)random->getU32() >> 26];
		*(_DWORD *)(v3 + v4) = ((v6 >> (((unsigned int)random->getU32() >> 27) & 0x18)) & 0xFF | *(_DWORD *)(v3 + v4)) << 8;
		v7 = lookupTable[(unsigned int)random->getU32() >> 26];
		*(_DWORD *)(v3 + v4) = ((v7 >> (((unsigned int)random->getU32() >> 27) & 0x18)) & 0xFF | *(_DWORD *)(v3 + v4)) << 8;
		v8 = lookupTable[(unsigned int)random->getU32() >> 26];
		uint32_t result = random->getU32();
		*(_DWORD *)(v3 + v4) |= (v8 >> (((unsigned int)result >> 27) & 0x18)) & 0xFF;
		v4 += 4LL;
	} while (v4 != 16);
}

int DecryptResourceFile(char* filePath, char* crcSeed)
{
	// Create buffers
	uint8_t keyBuffer[16];
	uint8_t ivBuffer[16];

	// Get the CRC32
	std::uint32_t crc32 = CRC::Calculate(crcSeed, strlen(crcSeed), CRC::CRC_32());

	// Create the SeadRandom instance
	SeadRandom random = SeadRandom(crc32);

	// Create the key and IV
	CreateByteSequence(&random, keyBuffer);
	CreateByteSequence(&random, ivBuffer);

	// Open a file input stream
	std::ifstream ifs(filePath, std::ios::binary | std::ios::ate);

	// Seek to the end of the file and get the length
	ifs.seekg(0, std::ios::end);
	int length = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	// Read into a buffer
	uint8_t* fileBytes = new uint8_t[length];
	ifs.read((char*)fileBytes, length);

	// Close the stream
	ifs.close();

	// Test for the "nisasyst" magic numbers
	if (memcmp(&fileBytes[length - 8], magicNumbers, 8) != 0)
	{
		std::cout << "Not a valid resource file.\n";
		return -1;
	}

	// Create the output buffer
	uint8_t* outputBuffer = new uint8_t[length - 8];

	// Decrypt the data
	int result = DecryptAes128Cbc(fileBytes, outputBuffer, length - 8, keyBuffer, ivBuffer);
	if (result != 0)
	{
		std::cout << "Failed to decrypt file (" << result << ")\n";
		return -2;
	}

	// Create the new name
	int newNameLength = strlen(filePath) + strlen(".decrypted") + 1;
	char* newName = new char[newNameLength];
	sprintf_s(newName, newNameLength, "%s.decrypted", filePath);

	// Create an output stream and write to it
	std::ofstream fout;
	fout.open(newName, std::ios::binary | std::ios::out);
	if (!fout)
	{
		std::cout << "Failed to open output file.";
		return -3;
	}

	// Write the data
	fout.write((const char*)outputBuffer, length);

	// Close the stream
	fout.close();

	return 0;
}

int DecryptSaveFile(char* filePath)
{
	// Open a file input stream
	std::ifstream ifs(filePath, std::ios::binary | std::ios::ate);

	// Seek to the end of the file and get the length
	ifs.seekg(0, std::ios::end);
	int length = ifs.tellg();
	int crypto_content_length = 0x40;
	ifs.seekg(0, std::ios::beg);

	// Read into a buffer
	uint8_t* fileBytes = new uint8_t[length];
	ifs.read((char*)fileBytes, length);

	// Close the stream
	ifs.close();

	// Create buffers
	uint8_t ciphertext[0x483A0];
	uint8_t plaintext[0x483A0];
	uint8_t keyBuffer[16];
	uint8_t ivBuffer[16];

	// Get seeds for SeadRandom
	uint32_t seedOne = *(unsigned int *)(fileBytes + 0x483C0);
	uint32_t seedTwo = *(unsigned int *)(fileBytes + 0x483C4);
	uint32_t seedThree = *(unsigned int *)(fileBytes + 0x483C8);
	uint32_t seedFour = *(unsigned int *)(fileBytes + 0x483CC);

	// Create a new SeadRandom instance
	SeadRandom random = SeadRandom(seedOne, seedTwo, seedThree, seedFour);

	// Copy the ciphertext and IV into the buffers
	memcpy(ciphertext, fileBytes + 16, 0x483A0);
	memcpy(ivBuffer, fileBytes + 0x483B0, 16);

	// Create the key
	CreateSaveKey(keyBuffer, &random);

	// Attempt to decrypt the file
	int ret = DecryptAes128Cbc(ciphertext, plaintext, 0x483A0, keyBuffer, ivBuffer);
	if (ret != 0)
	{
		std::cout << "Failed to decrypt save file (" << ret << ")\n";
		return -1;
	}

	// Create the new name
	int newNameLength = strlen(filePath) + strlen(".decrypted") + 1;
	char* newName = new char[newNameLength];
	sprintf_s(newName, newNameLength, "%s.decrypted", filePath);

	// Create an output stream and write to it
	std::ofstream fout;
	fout.open(newName, std::ios::binary | std::ios::out);
	if (!fout)
	{
		std::cout << "Failed to open output file.";
		return -3;
	}

	// Write the data
	// First header
	fout.write((const char*)fileBytes, 0x10);

	// Then savedata
	fout.write((const char*)plaintext, length - crypto_content_length);

	// Close the stream
	fout.close();

	return 0;
}

void PrintUsage()
{
	std::cout << "Decrypting resource files:\n";
	std::cout << "BlitzResourceDecryptor.exe resource <file> <game path>\n\n";
	std::cout << "\"Game path\" is the path the game would have used to access the file.\n";
	std::cout << "For example, the game path for \"NpcInfo.byml\" would be \"Mush/NpcInfo.byml\".\n\n";
	std::cout << "Decrypting save files:\n";
	std::cout << "BlitzResourceDecryptor.exe save <file>\n";
}

int main(int argc, char *argv[])
{
	std::cout << "BlitzResourceDecryptor for Splatoon 2 v2.3.3 by OatmealDome\n\n";

	// Check if a decryption type wasn't even chosen
	if (argc < 2)
	{
		// Print usage
		PrintUsage();
		return 1;
	}

	// Get the decryption type
	char* decryptionType = argv[1];

	if (strcmp(decryptionType, "resource") == 0)
	{
		// Check for valid number of arguments
		if (argc != 4)
		{
			// Print usage
			PrintUsage();
			return 1;
		}

		// Attempt to decrypt the file
		if (DecryptResourceFile(argv[2], argv[3]) == 0)
		{
			// Output the result
			std::cout << "Wrote file successfully.\n";

			// Success
			return 0;
		}
		else
		{
			// Not a success.
			return 1;
		}
	}
	else if (strcmp(decryptionType, "save") == 0)
	{
		// Check for valid number of arguments
		if (argc != 3)
		{
			// Print usage
			PrintUsage();
			return 1;
		}

		// Attempt to decrypt the file
		if (DecryptSaveFile(argv[2]) == 0)
		{
			// Output the result
			std::cout << "Wrote save file successfully.\n";

			// Success
			return 0;
		}
		else
		{
			// Not a success.
			return 1;
		}
	}
	else
	{
		PrintUsage();
		return 1;
	}

	return 0;
}
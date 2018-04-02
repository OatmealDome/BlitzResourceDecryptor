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

int main(int argc, char *argv[])
{
	std::cout << "BlitzResourceDecryptor for Splatoon 2 v2.3.0 by OatmealDome\n\n";
	if (argc != 3)
	{
		std::cout << "Usage:\n";
		std::cout << "BlitzResourceDecryptor.exe <file> <game path>\n\n";
		std::cout << "\"Game path\" is the path the game would have used to access the file.\n";
		std::cout << "For example, the game path for \"NpcInfo.byml\" would be \"Mush/NpcInfo.byml\".\n";
		return 1;
	}

	// Create buffers
	uint8_t keyBuffer[16];
	uint8_t ivBuffer[16];

	// Get the CRC32
	std::uint32_t crc32 = CRC::Calculate(argv[2], strlen(argv[2]), CRC::CRC_32());

	// Create the SeadRandom instance
	SeadRandom random = SeadRandom(crc32);

	// Create the key and IV
	CreateByteSequence(&random, keyBuffer);
	CreateByteSequence(&random, ivBuffer);

	// Open a file input stream
	std::ifstream ifs(argv[1], std::ios::binary | std::ios::ate);

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
		return 1;
	}

	// Create the output buffer
	uint8_t* outputBuffer = new uint8_t[length - 8];

	// Decrypt the data
	int result = DecryptAes128Cbc(fileBytes, outputBuffer, length - 8, keyBuffer, ivBuffer);
	if (result != 0)
	{
		std::cout << "Failed to decrypt file (" << result << ")\n";
		return 1;
	}

	// Create the new name
	int newNameLength = strlen(argv[1]) + strlen(".decrypted") + 1;
	char* newName = new char[newNameLength];
	sprintf_s(newName, newNameLength, "%s.decrypted", argv[1]);

	// Create an output stream and write to it
	std::ofstream fout;
	fout.open(newName, std::ios::binary | std::ios::out);
	if (!fout)
	{
		std::cout << "Failed to open output file.";
		return 1;
	}

	// Write the data
	fout.write((const char*)outputBuffer, length);

	// Close the stream
	fout.close();

	// Output the result
	std::cout << "Wrote file successfully.\n";

	return 0;
}
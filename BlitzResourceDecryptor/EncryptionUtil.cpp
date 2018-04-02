#include "stdafx.h"

#include <openssl/evp.h>

#include "EncryptionUtil.h"

int DecryptAes128Cbc(uint8_t* sourceBuffer, uint8_t* destBuffer, int sourceSize, uint8_t* key, uint8_t* iv)
{
	EVP_CIPHER_CTX *ctx;

	//memset(destBuffer, 0, sourceSize);

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		return -1;

	/* Initialise the decryption operation. IMPORTANT - ensure you use a key
	* and IV size appropriate for your cipher
	* In this example we are using 128 bit AES (i.e. a 128 bit key). The
	* IV size for *most* modes is the same as the block size. For AES this
	* is 128 bits */
	if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
		return -2;

	EVP_CIPHER_CTX_set_key_length(ctx, 128);

	/* Provide the message to be decrypted, and obtain the plaintext output.
	* EVP_DecryptUpdate can be called multiple times if necessary
	*/
	int decryptedLength;
	if (1 != EVP_DecryptUpdate(ctx, destBuffer, &decryptedLength, sourceBuffer, sourceSize))
		return -3;

	/* Finalize the decryption. Further plaintext bytes may be written at
	* this stage.
	*/
	int pad_len;
	if (1 != EVP_DecryptFinal_ex(ctx, destBuffer + decryptedLength, &decryptedLength))
		return -4;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return 0;
}

#ifndef CHUNK_CLS
#define CHUNK_CLS

#include "library.h"

class Chunk{
	private:
		bool shouldFree;

	public:
		char *plaintext;
		int size;

		Chunk();
		~Chunk();


};

class EncryptedChunk{
	private:
		bool shouldFree;

	public:
		char *ciphertext;
		int size;

		EncryptedChunk();
		~EncryptedChunk();


};

#endif
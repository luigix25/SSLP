#include "Chunk.h"

Chunk::Chunk(){
	plaintext = NULL;
	size = 0;
	shouldFree = true;
}
Chunk::~Chunk(){
	if(plaintext != NULL && shouldFree)
		delete[] plaintext;
}

EncryptedChunk::EncryptedChunk(){
	ciphertext = NULL;
	size = 0;
	shouldFree = true;

}
EncryptedChunk::~EncryptedChunk(){
	if(ciphertext != NULL && shouldFree)
		delete[] ciphertext;
}
#include "Chunk.h"

Chunk::Chunk(){
	plaintext = NULL;
	size = 0;
	shouldFree = false;
}

Chunk::~Chunk(){
	clean();

}

void Chunk::clean(){
	if(plaintext != NULL && shouldFree){
		delete[] plaintext;
		plaintext = NULL;
		shouldFree = false;
	}
}

void Chunk::setPlainText(char* data){
	clean();
	plaintext = data;
	shouldFree = true;
}

void Chunk::setPlainText(char* data, bool boolean){

	clean();
	plaintext = data;
	shouldFree = boolean;

}


char* Chunk::getPlainText(){

	return plaintext;

}

void Chunk::setInt(uint32_t &value){
	clean();
	shouldFree = false;					//can't delete an integer
	plaintext = (char*)&value;

}

uint32_t Chunk::getInt(){

	return *((uint32_t*)plaintext);

}

EncryptedChunk::EncryptedChunk(){
	ciphertext = NULL;
	size = 0;
	shouldFree = false;

}

EncryptedChunk::~EncryptedChunk(){
	if(ciphertext != NULL && shouldFree)
		delete[] ciphertext;
}

void EncryptedChunk::clean(){
	if(ciphertext != NULL && shouldFree){
		delete[] ciphertext;
		ciphertext = NULL;
		shouldFree = false;
	}
}


void EncryptedChunk::setCipherText(char* data){
	clean();
	shouldFree = true;
	ciphertext = data;
}

char* EncryptedChunk::getCipherText(){

	return ciphertext;

}

void EncryptedChunk::setInt(uint32_t &value){
	clean();
	shouldFree = false;					//can't delete an integer
	ciphertext = (char*)&value;
}

uint32_t EncryptedChunk::getInt(){

	return *((uint32_t*)ciphertext);


}
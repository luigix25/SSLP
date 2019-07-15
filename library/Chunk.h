#ifndef CHUNK_CLS
#define CHUNK_CLS

#include "library.h"

class Chunk{
	private:
		bool shouldFree;
		char *plaintext;
		void clean();

	public:
		int size;

		Chunk();
		~Chunk();

		void setPlainText(char*);
		void setPlainText(char*,bool);

		char* getPlainText();
		void setInt(uint32_t&);
		uint32_t getInt();


};

class EncryptedChunk{
	private:
		bool shouldFree;
		char *ciphertext;
		void clean();

	public:
		int size;

		EncryptedChunk();
		~EncryptedChunk();

		void setCipherText(char*);
		char* getCipherText();
		void setInt(uint32_t&);
		uint32_t getInt();


};

#endif
#ifndef WRITEFILEMANAGER
#define WRITEFILEMANAGER

#include "FileManager.h"

#include <fstream>
using namespace std;
class WriteFileManager : public FileManager {
	private:
		string original_file_name;
	protected:
		bool openStream();

	public:
		WriteFileManager(string &name, uint64_t size); //Constructor for write operation 
		WriteFileManager(const char *name, uint64_t size);
		file_status write(Chunk &c); //Write in append chunk
		void finalize(bool);
};

#endif
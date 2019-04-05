#ifndef FILEMANAGER
	#define FILEMANAGER
	#include "FileManager.h"
#endif   
#include <fstream>
using namespace std;
class WriteFileManager : public FileManager {
	protected:
		bool openStream();

	public:
		WriteFileManager(string &name, uint64_t size); //Constructor for write operation 
		WriteFileManager(const char *name, uint64_t size);
		file_status write(chunk *c); //Write in append chunk
};

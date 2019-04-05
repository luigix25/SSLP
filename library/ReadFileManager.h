#ifndef FILEMANAGER
	#define FILEMANAGER
	#include "FileManager.h"
#endif
#include <fstream>
using namespace std;
class ReadFileManager : public FileManager {
	protected:
		uint64_t compute_size_file();
		bool openStream();
		
	public:
		ReadFileManager(string &name); //Constructor for read operation
		ReadFileManager(const char *name);
		file_status read(chunk *c); //Read chunk by chunk sequentially after invocation
};

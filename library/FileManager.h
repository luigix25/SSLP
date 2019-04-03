#include "library.h"
using namespace std;
class FileManager {
	private:
		string file_name;
	public:
		FileManager(string name);
		FileManager(char *name);
		file_status read(chunk *c, uint32_t chunk_index);
		file_status write(chunk *c); 
		bool file_exists();
		uint64_t size_file(); // -1 file not exits
};

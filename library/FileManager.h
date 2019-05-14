#ifndef FILEMANAGER
	#define FILEMANAGER

#include <fstream>
#include "library.h"

using namespace std;
class FileManager {
	protected:
		string file_name;	
		fstream fs;
		uint64_t file_size;
		uint64_t remaining_size; 
		virtual bool openStream() = 0;
		void closeStream();		// cambiare nome, ambiguo con sotto
		
	public:
		FileManager(string &name, uint64_t size); //Constructor for write operation 
		~FileManager();
		bool file_exists(); //Check existance of file
		uint64_t size_file(); // -1 file not exits		
		void close_stream(); //Close the file stream
		void reset(); //The class restart from first chunk
		void changeName(string&);
};

#endif
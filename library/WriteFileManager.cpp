#include <iostream>
#include "WriteFileManager.h"
WriteFileManager::WriteFileManager(string &name, uint64_t size) : FileManager(name,size){}

WriteFileManager::WriteFileManager(const char *name, uint64_t size) : FileManager((string&)name,size){};

bool WriteFileManager::openStream(){
	if(!fs.is_open()){
		fs.open(file_name, fstream::out | fstream::app | fstream::binary);
		if(fs.fail()){ 
			cout << "LOG: file can't open" << endl;
			return false;
		}
	}
	return true;
} 

file_status WriteFileManager::write(chunk* c){ //write in append
	if(remaining_size <= 0){
		return END_OF_FILE;
	}
	if(!openStream()){ // Check if stream its open
		return FILE_ERROR;
	}
	fs.write(c->plaintext, c->size); // Write new block in to the file
	remaining_size -= c->size;
	closeStream();
	return NO_ERRORS; // All good
}
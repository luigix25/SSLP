#include <iostream>
#include "WriteFileManager.h"
WriteFileManager::WriteFileManager(string &name, uint64_t size) : FileManager(name,size){
	if(file_exists()){
		original_file_name = file_name;
		string tmp(".tmp");
		string new_name = file_name + tmp;
		changeName(new_name);
	} else{
		original_file_name = "";
	}
}

WriteFileManager::WriteFileManager(const char *name, uint64_t size) : WriteFileManager((string&)name,size){}

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
	if(!openStream()){ // Check if stream its open
		return FILE_ERROR;
	}
	fs.write(c->plaintext, c->size); // Write new block in to the file
	remaining_size -= c->size;
	closeStream();
	if(remaining_size <= 0){
		return END_OF_FILE;
	}
	return NO_ERRORS; // All good
}

void WriteFileManager::finalize(){
	if(remaining_size > 0){ // after error, remove file
		remove((const char*)file_name.c_str());
	}
	if(original_file_name != ""){ // end of write, substitute previous file with tmp
		remove((const char*)original_file_name.c_str());
		rename((const char*)file_name.c_str(),(const char*)original_file_name.c_str());
	}
}
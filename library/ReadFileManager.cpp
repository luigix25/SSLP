#include <iostream>
#include "ReadFileManager.h"
ReadFileManager::ReadFileManager(string &name) : FileManager(name,0){
	remaining_size = file_size = compute_size_file();
}

ReadFileManager::ReadFileManager(const char *name) : ReadFileManager((string&)name){}

//USE ONLY IN CONSTRUCTOR
uint64_t ReadFileManager::compute_size_file(){
	fs.open(file_name, fstream::in | fstream::binary);
	if(fs.fail()){ 
		cout << "LOG: file can't open" << endl;
		return false;
	}	
	fs.seekg (0,fs.end);
	uint64_t length = fs.tellg(); 
	fs.close();
	return length;
}

bool ReadFileManager::openStream(){
	if(!fs.is_open()){
		fs.open(file_name, fstream::in | fstream::binary);
		if(fs.fail()){ 
			cout << "LOG: file can't open" << endl;
			return false;
		}
	}
	return true;
} 

// Read at sector index in file and copy it into buffer
file_status ReadFileManager::read(Chunk &c){ 	

	if(!openStream()){
		return FILE_ERROR;
	}	
	uint64_t chunk_address = file_size - remaining_size; //secure, only internal modify
	uint64_t size; // curent chunk size
	if(remaining_size >= MAX_CHUNK_SIZE){
		size = MAX_CHUNK_SIZE;
	} else{
		size = remaining_size; //if last chunk have lower size
	}
	c.size = size;
	fs.seekg(chunk_address); // Set position on disk in input stream
	c.setPlainText(new char[size]);

	fs.read(c.getPlainText(), size);
	if(!fs){
		return FILE_ERROR;
	}

	remaining_size -= c.size;

	closeStream();
	
	if(remaining_size <= 0){
		return END_OF_FILE;
	}
	return NO_ERRORS;
}	
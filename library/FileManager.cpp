#include <iostream>
#include <fstream>
#include "FileManager.h"

FileManager::FileManager(string name){
	this->file_name = name;
}

FileManager::FileManager(char *name){
	this->file_name = name;
}

uint64_t FileManager::size_file(){ // If true, the file size is correct
	if(!file_exists()){
		return -1;
	}
	fstream fs;
	fs.open(file_name, fstream::in | fstream::binary); // Open stream
	fs.seekg (0,fs.end); // Move to the last byte
	uint64_t length = fs.tellg(); // Actual disk size
	fs.close(); // Close the stream
	return length;
}
bool FileManager::file_exists(){ // true if exists
	fstream fs;	
	fs.open(file_name, fstream::in);
	if(fs.fail()){ 
		return false;
	}
	return true;
}
// Read at sector index in file and copy it into buffer
file_status FileManager::read(chunk *c, uint32_t chunk_index){ 
	uint64_t chunk_address = chunk_index * MAX_CHUNK_SIZE;
	
	if(!file_exists()){ // Check if file exits
		return FILE_NOT_EXISTS;
	}
	
	uint64_t size_file = this->size_file();
	
	if(chunk_address < 0 || chunk_address >= size_file){ // Error, out of bound
		return OUT_OF_BOUND;
	}
	
	uint64_t size = size_file- chunk_address;
	if(size >= MAX_CHUNK_SIZE){ // Altrimenti in size trovo già la dimensione rimanente del file
		size = MAX_CHUNK_SIZE;
	}	
	c->size = size;
	fstream fs;	
	fs.open(file_name, fstream::in | fstream::binary); // Open in stream	
	if(!fs.is_open()){ // Check if stream its open
		fs.close(); // Close the stream
		return FILE_ERROR_OPEN;
	}
	fs.seekg(chunk_address); // Set position on disk in input stream
	fs.read(c->plaintext, size); // Read on file and copy the content in buffer
	fs.close();	// Close the stream
	return NO_ERRORS; // All good
}	


file_status FileManager::write(chunk* c){ //write in append
	cout << "LOG: start write " << endl;
	fstream fs;	
	fs.open(file_name, fstream::out | fstream::binary |fstream::app); // Open out stream	
	if(!fs.is_open()){ // Check if stream its open
		fs.close(); // Close the stream
		return FILE_ERROR_OPEN;
	}
	fs.write(c->plaintext, c->size); // Write new block in to the file
	cout << "LOG: scritti " << c->size << endl;	
	fs.close();	// Close the stream
	return NO_ERRORS; // All good
}

/*Note
	- fstream doesn't work with uint8_t, need to apply reinterpret_cast<char*>(buffer) before using read/write
*/
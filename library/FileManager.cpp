#include <iostream>
#include "FileManager.h"

FileManager::FileManager(string &name, uint64_t size = 0){
	this->file_name = name;
	this->file_size = size;
	this->remaining_size = this->file_size;
}

void FileManager::closeStream(){
	if(fs.is_open() && remaining_size <= 0){ //se il trasferimento è finito
		remaining_size = 0;
		fs.close();
	}
}

uint64_t FileManager::size_file(){ // If true, the file size is correct
	return file_size;
}

bool FileManager::file_exists(){ // true if exists
	fstream tmp(this->file_name);
	return tmp.good();
}

//Reset the internal counter for restart the operation
void FileManager::reset(){
	remaining_size = file_size;
}

//Close the stream
void FileManager::close_stream(){
	if(fs.is_open())
		fs.close();
}

void FileManager::changeName(string& name){
	file_name = name;
	cout << "NAME " << file_name << endl;
}

FileManager::~FileManager(){
	close_stream();
}
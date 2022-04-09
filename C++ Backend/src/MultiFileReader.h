#pragma once
#ifndef MFR_H
#define MFR_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <bit>
#include <cstdint>
#include "hashlibpp.h"
#include "Console.h"

class MultiFileReader {

public:

	//Initializes the multifile with the bytes from path fileName.
	//Modified version of read all bytes a from file from https://codereview.stackexchange.com/questions/22901/reading-all-bytes-from-a-file.
	static bool InitializeMultiFile(std::filesystem::path fileName);

	//Returns a null-terminated string of the contents of the file fileName, or nullptr if no such file exists in the multifile.
	//Only use this instead of the other ReadFile function if you know your file reader won't be ruined if the null terminator isn't in the proper place.
	static const char* ReadFile(const char* fileName);

	//Returns a null-terminated string of the contents of the file fileName, or nullptr if no such file exists in the multifile. Also sets outLength to the length of the file in bytes, and does not change its value if no file is found.
	static const char* ReadFile(const char* fileName, unsigned int& outLength);

	//Returns a null-terminated string of the contents of the file fileName, or nullptr if no such file exists in the multifile. Also sets outLength to the length of the file in bytes, and does not change its value if no file is found.
	static const char* ReadFile(const char* fileName, unsigned int* outLength);

	//Returns if the file with name "name" exists. 
	static bool Exists(const char* name);

	//Don't call this. It's a pretty bad idea.
	//You're going to call it now, aren't you?
	static void FreeMultifileBytes();

private:

	static char* __restrict multifileBytes;
	static size_t numFilePointerBytes;
	static hashwrapper* wrapper;
	static size_t fileSize;
};

#endif
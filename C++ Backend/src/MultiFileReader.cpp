#include "MultiFileReader.h"

size_t MultiFileReader::numFilePointerBytes;
size_t MultiFileReader::fileSize;
char* __restrict MultiFileReader::multifileBytes;
hashwrapper* MultiFileReader::wrapper = new md5wrapper();

bool MultiFileReader::InitializeMultiFile(std::filesystem::path fileName) {

	std::ifstream ifs = std::ifstream(fileName, std::ios::binary | std::ios::ate);

	try {

		const std::ifstream::pos_type pos = ifs.tellg();
		MultiFileReader::multifileBytes = new char[pos];
		MultiFileReader::fileSize = pos;

		ifs.seekg(0, std::ios::beg);
		ifs.read(&multifileBytes[0], pos);
		ifs.close();

		MultiFileReader::numFilePointerBytes = *(unsigned int*)&MultiFileReader::multifileBytes[0];
		if (MultiFileReader::numFilePointerBytes % 24 != 4) { //do a basic check to make sure the multifile is at least somewhat valid

			Console::WriteLine("Malformed multifile header!");
			return false;
		}
	} catch (std::ifstream::failure exc) {
		
		Console::stream << exc.what() << " " << exc.code() << '\n';
		return false;
	}

	return true;
}

const char* MultiFileReader::ReadFile(const char* fileName) {

	std::string hash = wrapper->getHashFromString(fileName);
	std::string fileHash;

	char* hashBytes = new char[17]; //make a buffer for the hash bytes
	hashBytes[16] = '\0'; //don't forget to null-terminate it

	for (unsigned int i = 4; i < MultiFileReader::numFilePointerBytes; i += 24) {

		if (i + 16 > (MultiFileReader::numFilePointerBytes - 1)) goto fileNotFound;

		errno_t err = memcpy_s(hashBytes, 16, &MultiFileReader::multifileBytes[i], 16); //copy the hash bytes for comparison
		if (err != 0) goto fileNotFound;

		fileHash = hashBytes;
		if (hash == fileHash) {

			if (i + 20 > (numFilePointerBytes - 1)) goto fileNotFound; //make sure there's enough space in the pointer bytes to read everything we need

			unsigned int pointer = *(unsigned int*)&MultiFileReader::multifileBytes[i + 16];
			unsigned int length = *(unsigned int*)&MultiFileReader::multifileBytes[i + 20];

			if (length == 0 || pointer == 0 || (pointer + length) > MultiFileReader::fileSize) goto fileNotFound; //do some basic checks to make sure some kind of buffer overrun won't occur

			delete[] hashBytes;
			return &MultiFileReader::multifileBytes[pointer];
		}
	}

fileNotFound:

	delete[] hashBytes; //free the hash bytes

	Console::stream << "File \"" << fileName << "\" was not found!" << '\n';
	return nullptr;
}

const char* MultiFileReader::ReadFile(const char* fileName, unsigned int& outLength) {

	std::string hash = wrapper->getHashFromString(fileName);
	std::string fileHash;

	char* hashBytes = new char[17]; //make a buffer for the hash bytes
	hashBytes[16] = '\0'; //don't forget to null-terminate it

	for (unsigned int i = 4; i < MultiFileReader::numFilePointerBytes; i += 24) {

		if (i + 16 > (MultiFileReader::numFilePointerBytes - 1)) goto fileNotFound;

		errno_t err = memcpy_s(hashBytes, 16, &MultiFileReader::multifileBytes[i], 16); //copy the hash bytes for comparison
		if (err != 0) goto fileNotFound;

		fileHash = hashBytes;
		if (hash == fileHash) {

			if (i + 20 > (numFilePointerBytes - 1)) goto fileNotFound; //make sure there's enough space in the pointer bytes to read everything we need

			unsigned int pointer = *(unsigned int*)&MultiFileReader::multifileBytes[i + 16];
			unsigned int length = *(unsigned int*)&MultiFileReader::multifileBytes[i + 20];

			if (length == 0 || pointer == 0 || (pointer + length) > MultiFileReader::fileSize/* ||
				MultiFileReader::multifileBytes[pointer + length] != '\0'*/) goto fileNotFound; //do some basic checks to make sure some kind of buffer overrun won't occur
			outLength = length;
			delete[] hashBytes;

			return &MultiFileReader::multifileBytes[pointer];
		}
	}

fileNotFound:

	delete[] hashBytes; //free the hash bytes

	Console::stream << "File \"" << fileName << "\" was not found!" << '\n';
	return nullptr;
}

const char* MultiFileReader::ReadFile(const char* fileName, unsigned int* outLength) {

	std::string hash = wrapper->getHashFromString(fileName);
	std::string fileHash;

	char* hashBytes = new char[17]; //make a buffer for the hash bytes
	hashBytes[16] = '\0'; //don't forget to null-terminate it

	for (unsigned int i = 4; i < MultiFileReader::numFilePointerBytes; i += 24) {

		if (i + 16 > (MultiFileReader::numFilePointerBytes - 1)) goto fileNotFound;

		errno_t err = memcpy_s(hashBytes, 16, &MultiFileReader::multifileBytes[i], 16); //copy the hash bytes for comparison
		if (err != 0) goto fileNotFound;

		fileHash = hashBytes;
		if (hash == fileHash) {

			if (i + 20 > (numFilePointerBytes - 1)) goto fileNotFound; //make sure there's enough space in the pointer bytes to read everything we need

			unsigned int pointer = *(unsigned int*)&MultiFileReader::multifileBytes[i + 16];
			unsigned int length = *(unsigned int*)&MultiFileReader::multifileBytes[i + 20];

			if (length == 0 || pointer == 0 || (pointer + length) > MultiFileReader::fileSize) goto fileNotFound; //do some basic checks to make sure some kind of buffer overrun won't occur
			*outLength = length;
			delete[] hashBytes;

			return &MultiFileReader::multifileBytes[pointer];
		}
	}

fileNotFound:

	delete[] hashBytes; //free the hash bytes

	Console::stream << "File \"" << fileName << "\" was not found!" << '\n';
	return nullptr;
}

bool MultiFileReader::Exists(const char* name) {
	
	std::string hash = wrapper->getHashFromString(name);
	std::string fileHash;

	char* hashBytes = new char[17];
	hashBytes[16] = '\0';

	for (unsigned int i = 4; i < MultiFileReader::numFilePointerBytes; i += 24) {

		if (i + 16 > (MultiFileReader::numFilePointerBytes - 1)) return false;

		errno_t err = memcpy_s(hashBytes, 16, &MultiFileReader::multifileBytes[i], 16);
		if (err != 0) { delete[] hashBytes; return false; }

		fileHash = hashBytes;
		if (hash == fileHash) { delete[] hashBytes; return true; }
	}

	delete[] hashBytes;
	return false;
}

void MultiFileReader::FreeMultifileBytes() {
	
	delete[] multifileBytes;
}
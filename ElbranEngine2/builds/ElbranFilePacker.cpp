#include <iostream>
#include <Windows.h>
#include <stdint.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

/* pack file format:
	uint32: version number
	uint32: file count
	uint32: byte location of file starts table
	uint8[?]: file binaries
	uint32[file count]: table containing the start bytes of the files in order
	char[?]: null-terminated file names in order
*/

constexpr int FILES_NAMES_BUFFER_SIZE = 100000;
constexpr int MAX_PACK_FILE_SIZE = 1000000000;
constexpr int MAX_FILES_PACKED = 1000;
char fileNames[FILES_NAMES_BUFFER_SIZE] = {};
uint32_t fileStartBytes[MAX_FILES_PACKED];
int nextFileNamesIndex = 0;
uint32_t filesPacked = 0;
uint32_t nextFileBufferIndex = 3*sizeof(uint32_t); // leave space for header containing 3 uint32s

enum class ErrorType {
	None,
	FileCountLimit,
	FileSizeLimit,
	FileNamesLimit,
	ReadFailed
};

ErrorType PackFilesWithin(const char* directory, unsigned char* fileBuffer) {
	char path[MAX_PATH] = {};
	StringCchCopyA(path, MAX_PATH, directory);
	size_t directoryNamelength = 0;
	StringCchLengthA(path, MAX_PATH, &directoryNamelength);
	StringCchCatA(path, MAX_PATH, "\\*");
	directoryNamelength++; // maintain trailing slash but not asterisk

	WIN32_FIND_DATAA findData;
	HANDLE findHandle = FindFirstFileA(path, &findData);
	if(findHandle == INVALID_HANDLE_VALUE) return ErrorType::None; // empty directory
	bool hasNextHandle = true;
	while(hasNextHandle) {
		path[directoryNamelength] = 0;
		StringCchCatA(path, MAX_PATH, findData.cFileName);

		if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(lstrcmpA(findData.cFileName, ".") != 0 && lstrcmpA(findData.cFileName, "..") != 0) {
				ErrorType recurError = PackFilesWithin(path, fileBuffer);
				if(recurError != ErrorType::None) return recurError;
			}
		}
		else {
			// check if this file can fit into the output
			size_t nameLength = FILES_NAMES_BUFFER_SIZE;
			StringCchLengthA(findData.cFileName, MAX_PATH, &nameLength);
			nameLength++; // include null-terminator
			
			if(findData.nFileSizeHigh > 0 || nextFileBufferIndex + findData.nFileSizeLow > MAX_PACK_FILE_SIZE) return ErrorType::FileSizeLimit;
			if(filesPacked >= MAX_FILES_PACKED) return ErrorType::FileCountLimit;
			if(nextFileNamesIndex + nameLength > FILES_NAMES_BUFFER_SIZE) return ErrorType::FileNamesLimit;

			// load and append the file data
			fileStartBytes[filesPacked] = nextFileBufferIndex;
			StringCchCopyA(fileNames + nextFileNamesIndex, FILES_NAMES_BUFFER_SIZE - nextFileNamesIndex, findData.cFileName);

			HANDLE fileHandle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
			DWORD bytesRead = 0;
			bool readSucceeded = ReadFile(fileHandle, fileBuffer + nextFileBufferIndex, findData.nFileSizeLow, &bytesRead, 0);
			CloseHandle(fileHandle);
			if(!readSucceeded || bytesRead != findData.nFileSizeLow) return ErrorType::ReadFailed;

			nextFileNamesIndex += nameLength;
			nextFileBufferIndex += findData.nFileSizeLow;
			filesPacked++;
		}

		hasNextHandle = FindNextFileA(findHandle, &findData);
	}

	FindClose(findHandle);
	return ErrorType::None;
}

int main(int numArgs, char* args[]) {
	if(numArgs != 3) { 
		std::cout << "\nArgument 1: relative path containing files to be packed; Argument 2: relative path and file name of the output pack file\n";
		return -1;
	}

	// validate input
	char exePath[MAX_PATH];
	GetModuleFileNameA(0, exePath, MAX_PATH);
	char* lastSlash = strrchr(exePath, '\\');
	if(!lastSlash) {
		std::cout << "\nEcountered an error finding the end of the executable path\n";
		return -1;
	}
	*(lastSlash + 1) = 0; // null terminate after the last slash to get rid of the .exe file name
	size_t exePathLength = MAX_PATH;
	HRESULT hResult = StringCchLengthA(args[0], MAX_PATH, &exePathLength);
	if(hResult != S_OK) {
		std::cout << "\nEncountered an error determining the length of the inputs\n";
		return -1;
	}
	
	size_t relativePathLength = MAX_PATH;
	for(int i = 1; i <= 2; i++) {
		HRESULT hResult = StringCchLengthA(args[i], MAX_PATH, &relativePathLength);

		if(hResult != S_OK) {
			std::cout << "\nEncountered an error determining the length of the inputs\n";
			return -1;
		}
		if(relativePathLength + exePathLength + 3 > MAX_PATH) {
			std::cout << "\nAbsolute file path is too long\n";
			return -1;
		}
	}

	// convert relative paths to absolute paths
	char inputPath[MAX_PATH] = {};
	char outputPath[MAX_PATH] = {};
	StringCchCopyA(inputPath, MAX_PATH, exePath);
	StringCchCopyA(outputPath, MAX_PATH, exePath);
	StringCchCatA(inputPath, MAX_PATH, args[1]);
	StringCchCatA(outputPath, MAX_PATH, args[2]);

	DWORD attributes = GetFileAttributesA(inputPath);
	if(attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
		std::cout << "\nInput directory not found\n";
		return -1;
	}

	// load all the file data
	unsigned char* fileBuffer = new unsigned char[MAX_PACK_FILE_SIZE];
	ErrorType packError = PackFilesWithin(inputPath, fileBuffer);
	switch(packError) {
	case ErrorType::FileCountLimit:
		std::cout << "\nError: Exceeded file count limit\n";
		return -1;
	case ErrorType::FileNamesLimit:
		std::cout << "\nError: Length of all file names was too long\n";
		return -1;
	case ErrorType::FileSizeLimit:
		std::cout << "\nError: Total pack file size was too large\n";
		return -1;
	case ErrorType::ReadFailed:
		std::cout << "\nEncountered an error reading a file\n";
		return -1;

	}

	uint32_t fileSize = nextFileBufferIndex + sizeof(uint32_t) * filesPacked + nextFileNamesIndex;
	if(fileSize > MAX_PACK_FILE_SIZE) {
		std::cout << "\nError: Total pack file size was too large\n";
		return -1;
	}

	// combine data buffers into one
	*((uint32_t*)fileBuffer) = 0; // version
	*((uint32_t*)(fileBuffer + sizeof(uint32_t))) = filesPacked;
	*((uint32_t*)(fileBuffer + 2*sizeof(uint32_t))) = nextFileBufferIndex;
	memcpy(fileBuffer + nextFileBufferIndex, fileStartBytes, sizeof(uint32_t) * filesPacked);
	memcpy(fileBuffer + nextFileBufferIndex + sizeof(uint32_t) * filesPacked, fileNames, nextFileNamesIndex);

	// save final file
	HANDLE fileHandle = CreateFileA(outputPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	if(fileHandle == INVALID_HANDLE_VALUE) {
		std::cout << "\nFailed to save pack file\n";
		return -1;
	}
	DWORD writtenBytes = 0;
	bool success = WriteFile(fileHandle, fileBuffer, fileSize, &writtenBytes, 0);
	CloseHandle(fileHandle);
	if(!success || writtenBytes != fileSize) {
		std::cout << "\nFailed to save pack file\n";
		return -1;
	}

	delete[] fileBuffer;
	std::cout << "\nSucceeded in pack file creation\n";
	return 0;
}
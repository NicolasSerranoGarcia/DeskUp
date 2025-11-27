#include "desk_up_lookup_table/desk_up_lookup_table.h"

#include <vector>
#include "exceptions/desk_up_file_not_open_exception/desk_up_file_not_open_exception.h"
#include "exceptions/desk_up_file_not_clear_exception/desk_up_file_not_clear_exception.h"

namespace fs = std::filesystem;

using namespace DeskUp::Utils;

using namespace DeskUp::Exceptions;

//format:
//1 - Number representing the total lines (not including this one)
//Consecutive (each line) - Any combination of characters (non-wide). Empty line (just "\n") is also a valid entry (which represents it is
//deleted), even if it is the last line. Note that the class doesn't permit creating an entry with an empty name in first place, so empty lines are deleted
//lines, and should be avoided when looking up and performin other operations
//What is not valid is a combination of characters and endl, as it would break the class system.
//Empty lines with just an "\n" are considered valid, according to the DeskUp usage of this class.
//Example (example.txt):
//4
//name1
//rgto 34 name
//
//this-is valid + as `^long´´ as } there - is @#no endl
bool LookUpTable::hasValidFormat(){
	if (!lookUpFile.is_open()) {
		return false;
	}

	// Save current position
	auto currentPos = lookUpFile.tellg();
	lookUpFile.clear();
	lookUpFile.seekg(0);

	std::string firstLine;
	if (!std::getline(lookUpFile, firstLine)) {
		// Empty file or read error
		lookUpFile.clear();
		lookUpFile.seekg(currentPos);
		return false;
	}

	// First line should be a number
	try {
		size_t count = std::stoull(firstLine);
		// Count actual lines
		size_t actualLines = 0;
		std::string line;
		while (std::getline(lookUpFile, line)) {
			actualLines++;
		}

		// Restore position
		lookUpFile.clear();
		lookUpFile.seekg(currentPos);

		return actualLines == count;
	} catch (...) {
		// First line is not a valid number
		lookUpFile.clear();
		lookUpFile.seekg(currentPos);
		return false;
	}
}

//operates over lookUpFile, and throws FileNotOpenException if it cannot open it with the specified mode
void LookUpTable::openFile(const fs::path& path, std::ios_base::openmode mode){
	lookUpFile.open(path, mode);

	if(!lookUpFile.is_open()){
		throw FileNotOpenException(path.string());
	}
}

bool LookUpTable::clearFile(){
	if (!lookUpFile.is_open()) {
		return false;
	}

	// Close and reopen in truncate mode
	lookUpFile.close();
	std::ofstream temp(filePath, std::ios::trunc);
	if (!temp.is_open()) {
		return false;
	}

	// Write initial count of 0
	temp << "0";
	temp.close();

	// Reopen in original mode
	const_cast<std::fstream&>(lookUpFile).open(filePath, std::ios::out | std::ios::in | std::ios::app);
	return lookUpFile.is_open();
}

// line is 0-indexed
std::string LookUpTable::getLineContent(const Line line){
	if(line > numEntries){
		throw std::runtime_error("Invalid line!");
	}

	std::string s;
	unsigned int i = 0;
	while(getline(lookUpFile, s)){
		if(i == line){
			break;
		}
		i++;
	}

	lookUpFile.clear();
	lookUpFile.seekg(0);

	return s;
}


LookUpTable::LookUpTable(const fs::path& pathToFile){

	numEntries = 0;

	//if file is empty or doesn't exist, create a new file
	if(pathToFile.empty()){
		static unsigned int fileNum = 0;
		const fs::path p("table" + std::to_string(fileNum));
		fileNum++;
		openFile(p, std::ios::out | std::ios::in | std::ios::trunc);
		this->filePath = p;
		lookUpFile << "0";
		lookUpFile.flush();
		return;
	}

	// If file doesn't exist, create it at the specified path
	if(!fs::exists(pathToFile)){
		// Create parent directories if needed
		if(pathToFile.has_parent_path()){
			std::error_code ec;
			fs::create_directories(pathToFile.parent_path(), ec);
		}
		openFile(pathToFile, std::ios::out | std::ios::in | std::ios::trunc);
		this->filePath = pathToFile;
		lookUpFile << "0";
		lookUpFile.flush();
		return;
	}

	// File exists but is not a regular file
	if(!fs::is_regular_file(pathToFile)){
		static unsigned int fileNum = 0;
		const fs::path p("table" + std::to_string(fileNum));
		fileNum++;
		openFile(p, std::ios::out | std::ios::in | std::ios::trunc);
		this->filePath = p;
		lookUpFile << "0";
		lookUpFile.flush();
		return;
	}

	openFile(pathToFile, std::ios::out | std::ios::in | std::ios::app);

	this->filePath = pathToFile;

	if(!hasValidFormat()){
		if(!clearFile()){
			throw FileNotClearException(pathToFile.string());
		}
		return;
	}

	// Read numEntries from the first line
	std::string firstLine = getLineContent(0);
	try {
		numEntries = std::stoull(firstLine);
	} catch (...) {
		// If we can't parse it, clear the file
		if (!clearFile()) {
			throw FileNotClearException(pathToFile.string());
		}
		numEntries = 0;
	}
}


size_t LookUpTable::getNumEntries(){
	return numEntries;
}


size_t LookUpTable::increaseNumEntries(){
	return ++numEntries;
}

size_t LookUpTable::decreaseNumEntries(){
	return --numEntries;
}

fs::path LookUpTable::addEntry(const std::string_view& entry) {
	if (entry.empty()) {
		throw std::runtime_error("Entry cannot be empty!");
	}

	// Check if entry already exists
	try {
		//throws if it can't find it
		getLine(entry);
		// If we get here, entry already exists
		throw std::runtime_error("Entry already exists in table");
	} catch (const std::runtime_error& e) {
		// Entry doesn't exist, which is what we want
		if (std::string(e.what()).find("No such entry") == std::string::npos) {
			throw; // Re-throw if it's a different error
		}
	}

	lookUpFile.clear();
	lookUpFile.seekp(0, std::ios::end);

	lookUpFile.put('\n');
	lookUpFile.write(entry.data(), entry.size());
	lookUpFile.flush();

	if (lookUpFile.fail()) {
		throw std::runtime_error("The new entry could not be added");
	}

	size_t newCount = increaseNumEntries();
	updateNumEntriesInFile();

	// The line number is the count (since we just added it as the last entry)
	Line lineNumber = static_cast<Line>(newCount);

	// Create associated directory
	fs::path dirPath = filePath.parent_path() / std::to_string(lineNumber);
	std::error_code ec;
	fs::create_directories(dirPath, ec);
	if (ec) {
		throw std::runtime_error("Failed to create directory for entry: " + ec.message());
	}

	return dirPath;
}

fs::path LookUpTable::deleteEntry(const std::string_view& entry) {
	if (entry.empty()) {
		throw std::runtime_error("Entry cannot be empty!");
	}

	// Get directory path before deleting entry
	fs::path dirPath = getDirFromEntry(entry);

	// Read all lines into memory (preserve total line count)
	lookUpFile.clear();
	lookUpFile.seekg(0);

	std::string countLine;
	if (!std::getline(lookUpFile, countLine)) {
		throw std::runtime_error("Failed to read file");
	}

	std::vector<std::string> lines;
	std::string line;
	bool found = false;
	Line deletedLine = 0;
	Line currentLine = 1;

	while (std::getline(lookUpFile, line)) {
		if (!found && line == entry) {
			found = true;
			deletedLine = currentLine;
			lines.push_back(""); // Keep the line, but empty its contents
		} else {
			lines.push_back(line);
		}
		currentLine++;
	}

	if (!found) {
		throw std::runtime_error("No such entry in the table: " + std::string(entry));
	}

	// Close and rewrite file (no trailing newline at EOF)
	lookUpFile.close();
	std::ofstream outFile(filePath, std::ios::trunc);
	if (!outFile.is_open()) {
		throw std::runtime_error("Failed to reopen file for writing");
	}

	// Do NOT change numEntries: a deleted entry leaves an empty line
	outFile << numEntries;
	if (!lines.empty()) {
		outFile << '\n';
		// Write first line without preceding extra newline beyond separator
		outFile << lines[0];
		for (size_t i = 1; i < lines.size(); ++i) {
			outFile << '\n' << lines[i];
		}
	}
	outFile.close();

	// Reopen in original mode
	lookUpFile.open(filePath, std::ios::out | std::ios::in | std::ios::app);
	if (!lookUpFile.is_open()) {
		throw std::runtime_error("Failed to reopen file");
	}

	// Delete associated directory
	std::error_code ec;
	fs::remove_all(dirPath, ec);
	if (ec) {
		throw std::runtime_error("Failed to delete directory for entry: " + ec.message());
	}

	return dirPath;
}

fs::path LookUpTable::getDirFromEntry(const std::string_view& entry){
	// Use parent directory of the file, not the file itself
	return filePath.parent_path() / std::to_string(getLine(entry));
}

//should be used by deleteEntry and getDirFromEntry
Line LookUpTable::getLine(const std::string_view& entry){
	if (entry.empty()) {
		throw std::runtime_error("Entry cannot be empty!");
	}

	// Save current position
	auto currentPos = lookUpFile.tellg();
	lookUpFile.clear();
	lookUpFile.seekg(0);

	// Skip first line (count)
	std::string firstLine;
	if (!std::getline(lookUpFile, firstLine)) {
		lookUpFile.clear();
		lookUpFile.seekg(currentPos);
		throw std::runtime_error("Failed to read file");
	}

	bool found = false;
	std::string s;
	Line lineNumber = 1; // Start from 1 (after the count line)

	while (std::getline(lookUpFile, s)) {
		if (s == entry) {
			found = true;
			break;
		}
		lineNumber++;
	}

	// Restore position
	lookUpFile.clear();
	lookUpFile.seekg(currentPos);

	// Logic was inverted - should throw if NOT found
	if (!found) {
		throw std::runtime_error("No such entry in the table: " + std::string(entry));
	}

	return lineNumber;
}

void LookUpTable::updateNumEntriesInFile(){
	if (!lookUpFile.is_open()) {
		return;
	}

	// We need to rewrite the entire file to safely update the first line
	// Save current position
	auto currentPos = lookUpFile.tellg();

	// Read all entries (skip first line)
	lookUpFile.clear();
	lookUpFile.seekg(0);

	std::string firstLine;
	std::getline(lookUpFile, firstLine); // Skip count line

	std::vector<std::string> entries;
	std::string line;
	while (std::getline(lookUpFile, line)) {
		entries.push_back(line);
	}

	// Close and rewrite file
	lookUpFile.close();
	std::ofstream outFile(filePath, std::ios::trunc);
	if (!outFile.is_open()) {
		// Try to reopen in original mode
		lookUpFile.open(filePath, std::ios::out | std::ios::in | std::ios::app);
		return;
	}

	// Write new count, no trailing newline unless there are entries
	outFile << numEntries;
	if (!entries.empty()) {
		outFile << '\n';
		// Write first entry
		outFile << entries[0];
		// Write remaining entries, each preceded by a newline separator
		for (size_t i = 1; i < entries.size(); ++i) {
			outFile << '\n' << entries[i];
		}
	}

	outFile.close();

	// Reopen in original mode (read/write/append)
	lookUpFile.open(filePath, std::ios::out | std::ios::in | std::ios::app);
	if (!lookUpFile.is_open()) {
		throw std::runtime_error("Failed to reopen file after update");
	}

	// Try to restore position (may not be exact due to first line change)
	lookUpFile.clear();
	if (currentPos != std::fstream::pos_type(-1)) {
		lookUpFile.seekg(currentPos);
	}
}

LookUpTable::~LookUpTable(){
	/*rewrite the number of lines again to the first line (although it should not be needed), just to be sure we are in sync*/

	updateNumEntriesInFile();

	lookUpFile.clear();
	lookUpFile.seekg(0);
	lookUpFile.seekp(0);
	lookUpFile.close();
}
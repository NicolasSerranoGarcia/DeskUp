#include "desk_up_lookup_table/desk_up_lookup_table.h"

#include "exceptions/desk_up_file_not_open_exception/desk_up_file_not_open_exception.h"
#include "exceptions/desk_up_file_not_clear_exception/desk_up_file_not_clear_exception.h"

namespace fs = std::filesystem;

using namespace DeskUp::Utils;

using namespace DeskUp::Exceptions;

//format:
//1 - Number representing the total lines (not including this one)
//Consecutive (each line) - Any combination of characters (non-wide). Empty line (just "\n") is also valid, even if it is the last line.
//What is not valid is a combination of characters and endl, as it would break the class system.
//Empty lines with just an "\n" are considered valid, according to the DeskUp usage of this class.
//Example (example.txt):
//4
//name1
//rgto 34 name
//
//this-is valid + as `^long´´ as } there - is @#no endl
//TODO: Implement
bool LookUpTable::hasValidFormat(){
	return true;
}

//operates over lookUpFile, and throws FileNotOpenException if it cannot open it with the specified mode
void LookUpTable::openFile(const fs::path& path, std::ios_base::openmode mode){
	lookUpFile.open(path, mode);

	if(!lookUpFile.is_open()){
		throw FileNotOpenException(path.string());
	}
}

//TODO: Implement
bool LookUpTable::clearFile(){
	return true;
}


LookUpTable::LookUpTable(const fs::path& pathToFile){

	//if file is empty or system non-valid, then create a new, empty file
	if(pathToFile.empty() || !fs::exists(pathToFile) || !fs::is_regular_file(pathToFile)){
		static unsigned int fileNum = 0;

		const fs::path p("table" + fileNum);

		openFile(p, std::ios::out | std::ios::in | std::ios::trunc);

		this->filePath = p;

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

	//TODO: Write the numEntries var from the first line.
	//Probably implement a getLineContent (int n) method to get the contents of line n

}


size_t LookUpTable::getNumEntries() const{
	return numEntries;
}


size_t LookUpTable::increaseNumEntries(){
	return ++numEntries;
}

size_t LookUpTable::decreaseNumEntries(){
	return --numEntries;
}


Line LookUpTable::addEntry(const std::string_view& entry){
	return 0;
}


Line LookUpTable::deleteEntry(const std::string_view& entry){
	return 0;
}

fs::path LookUpTable::getDirFromEntry(const std::string_view& entry) const{
	return filePath / std::string(std::to_string(getLine(entry)));
}

//should be used by deleteEntry and getDirFromEntry
Line LookUpTable::getLine(const std::string_view& entry) const{
	return 0;
}

LookUpTable::~LookUpTable(){
	/*rewrite the number of lines again to the first line (although it should not be needed), just to be sure we are in sync*/

	lookUpFile.close();
}
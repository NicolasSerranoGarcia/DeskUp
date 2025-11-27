#ifndef DESKUPLOOKUPTABLE_H
#define DESKUPLOOKUPTABLE_H

#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

//returned by some methods to indicate the line where the entry has been written
typedef unsigned int Line;

namespace DeskUp::Utils{

	//this class assumes the file you want to use as the underlying data has the following convention:
	//First line: number of total entries of the file (this first line not included)
	//Consecutive lines: the workspace name. Can be any combination of characters, as long as it is not a breakline
	//The associated directory is not specified, as it is implementation dependent. To get info on a directory, use the public
	// "getDir" method. Still, the name of the actual directory should only be used in case
	class LookUpTable final {

		public:
			LookUpTable(const fs::path& pathToFile);

			size_t getNumEntries();

			//also creates the directory
			fs::path addEntry(const std::string_view& entry);
			//also deletes the directory
			fs::path deleteEntry(const std::string_view& entry);

			fs::path getDirFromEntry(const std::string_view& entry);

			~LookUpTable();
		private:
			std::fstream lookUpFile;

			//for faster checks
			size_t numEntries;

			//save the file path for read-only operations
			fs::path filePath;

		private:

			size_t increaseNumEntries();
			size_t decreaseNumEntries();

			//Helpers that act over lookUpFile

		void openFile(const fs::path& path, std::ios_base::openmode mode);

		bool hasValidFormat();

		bool clearFile();

		std::string getLineContent(const Line line);

		void updateNumEntriesInFile();		//should be used by deleteEntry and getDir
		Line getLine(const std::string_view& entry);
	};
}


#endif
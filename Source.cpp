#include <iostream>
#include <boost/filesystem.hpp>
#include <iterator>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include "AnimeData.h"
#include "MALApiAccess.h"
#include <ctime>

using namespace boost::filesystem;

int filepicker(boost::filesystem::path SortTargetDirectory, std::vector<SortFiles> & mkvvector, boost::filesystem::path SortDestinationDirectory);
bool filesorter(std::vector <SortFiles> & mkvvector, boost::filesystem::path SortDestinationDirectory);
std::string FolderNameMake (std::string FileName);
bool metadatacollector(path SortDestinationDirectory);
bool IsAlpha(char a);
bool IsDigit(char a);


int main (int argc, char * argv[]) {

//Initialize input commands (using vector for target path and sort destination)	
	std::vector<std::string> commandline;
	commandline.assign(argv+1, argv + argc);
	path SortTargetDirectory;
	path SortDestinationDirectory;
	int FileCounter;
	int MALFlag = 0;

	if (argc == 1) { //For scrubs
		std::cout << "Correct usage is -t for the path to folder to be sorted and -d for the path to the save destination folder." << std::endl;
		std::cout << "Enclose your file path in \" \" if your path has spaces." << std::endl;
		return 0;
	}

	if (commandline[0].compare("-help") == 0) {
		std::cout << "Correct usage is -t for the path to folder to be sorted and -d for the path to the save destination folder." << std::endl;
		std::cout << "Enclose your file path in " " if your path has spaces." << std::endl;
		return 0;
	}	
	//Initialize the two path variables
	for (int i = 0; i != argc-1; i++) {		
			if (commandline[i].compare("-t") == 0) {
				SortTargetDirectory = commandline[i+1];				
				i++;
			}
			else if (commandline[i].compare("-d") == 0) {
				SortDestinationDirectory = commandline[i + 1];
				i++;
			}
			else if (commandline[i].compare("-m") == 0) {
				MALFlag = 1;				
			}

			else {
				std::cout << "Incorrect arguments. Please see -help.";
			}
	}

	//Path verification

	if (is_directory(SortTargetDirectory) == false) {
		std::cout << "Target directory(" << SortTargetDirectory <<") is an invalid directory." << std::endl;
		return 1;
	}
	else {
		std::cout << SortTargetDirectory << "is a valid directory." << std::endl;
	}

	if (is_directory(SortDestinationDirectory) == false) {
		std::cout << "Destination directory(" << SortDestinationDirectory <<") is an invalid directory." << std::endl;
		return 1;
	}
	else {
		std::cout << SortDestinationDirectory << "is a valid directory." << std::endl;
	}

	//Grab the .mkv files from a directory and write them into a log file
	std::vector<SortFiles> mkvvector;
	
	FileCounter = filepicker(SortTargetDirectory, mkvvector, SortDestinationDirectory);	
	if (FileCounter <= 0) {
		std::cout << "No files to sort in target directory." << std::endl;
		return 1;
	}
	std::cout << "Sorting " << FileCounter << " files..." << std::endl;


	//Make the folders based on the file name and move the files
	if (filesorter(mkvvector,SortDestinationDirectory) == false) return 1; 	

	//Option to grab info from MAL
	std::string UserName, Password;
	if (MALFlag == 1) {
		metadatacollector(SortDestinationDirectory);
	}
	

	
	//All done
	std::cout << "Sorting Complete!" << std::endl;
	
	/*Debugger*/
	// std::cout << SortTargetDirectory << std::endl;
	// std::cout << SortDestinationDirectory << std::endl;
	/*std::cout << "mkvvector: ";
	for (std::vector<SortFiles>::const_iterator itr = mkvvector.begin(); itr != mkvvector.end(); ++itr) {	
		std::cout << itr->FilePath << " " << itr->SortFolderName << std::endl;
	}

	std::cout << "mkvvectordirectory: ";
	for (std::vector<SortFiles>::const_iterator itr2 = mkvvectordirectory.begin(); itr2 != mkvvectordirectory.end(); ++itr2) {		
		std::cout << itr2->FilePath << " " << itr2->SortFolderName << std::endl;
	}*/

	return 0;
}

// Each file entry will be saved in a struct SortFiles containing the FilePath and the FileName. Function will iterate through the target directory and grab the pathes of all of the files which are .mkv and save them in a vector
int filepicker(boost::filesystem::path SortTargetDirectory, std::vector<SortFiles> & mkvvector, boost::filesystem::path SortDestinationDirectory) {
	int FileCounter = 0;
	SortFiles FileEntry;	

	//Save 
	for (directory_iterator itr(SortTargetDirectory); itr!= directory_iterator(); ++itr) {
		if(itr->path().extension() == ".mkv") {
			FileEntry.OriginalFilePath = itr->path();
			mkvvector.push_back(FileEntry);
			FileCounter++;
		} 		
	}
	
	std::ofstream outputfile;
	std::string DestinationPath;

//Write the vector to a log of some sort....
	DestinationPath = SortDestinationDirectory.string() + "\\log.txt";	
	//std::cout << DestinationPath << std::endl;
	
	outputfile.open(DestinationPath,std::ios::out | std::ios::trunc);	
	
	if(outputfile.bad() == true) {
		std::cout << "Log file write failed." << std::endl;
		return 0;
	}

	outputfile <<"Sorting " << FileCounter << " Files..." << std::endl;

	for (std::vector<SortFiles>::const_iterator itr = mkvvector.begin(); itr != mkvvector.end(); ++itr) {
		outputfile << itr->OriginalFilePath << std::endl;
	}
	outputfile.close();

	return FileCounter;
}
	

bool filesorter(std::vector <SortFiles> & mkvvector, boost::filesystem::path SortDestinationDirectory) {
	
	std::string FileName, FilePath, FolderName, FolderPath;		
	SortFiles FileEntry;

	//Initialize ofstream for writing a log of sorted files
	
	std::ofstream outputfile;
	std::string DestinationPath;
	
	DestinationPath = SortDestinationDirectory.string() + "\\Sorted Files.txt";
	outputfile.open(DestinationPath,std::ios::out | std::ios::trunc);

	if(outputfile.bad() == true) {
		std::cout << "Log file write failed." << std::endl;
		return false;
	}

	for (std::vector<SortFiles>::iterator itr = mkvvector.begin(); itr != mkvvector.end(); ++itr) {
		
		//Create the folder name by pruning out underscores/fansub group name/chksum/resolution no./episode/etc. from the file name and save it in both mkvvector and mkvvectordirectory

		FolderName = FolderNameMake(itr->OriginalFilePath.stem().string());				
		itr->AnimeName = FolderName;
		
		/*//Skip the file, if the file name somehow creates problems.
		else {
			continue;
		}*/

		
		//Build folder path
		FolderPath = SortDestinationDirectory.string() + "\\" + FolderName;
		path p(FolderPath);				
		
		//Create the folders if they don't exist and add a bit to the log file if the folder is actually created
		try {
			if (create_directory(p) == true) {
				outputfile << std::endl;				
				outputfile << "Directory: " << FolderName << std::endl;
			}

		}

		catch (filesystem_error& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			continue;
		}

		//Build Sorted File path
		FilePath = FolderPath + "\\" + itr->OriginalFilePath.filename().string();
		path g(FilePath);

		itr->SortedFilePath = g;

		//Move the files using the newly created SortedFilePath and write a log of the sorted files
		std::cout << "Now sorting: " << itr -> OriginalFilePath << std::endl;
		
		try {
			rename(itr->OriginalFilePath, itr->SortedFilePath);
		}

		catch (filesystem_error& e) {
			std::cerr << "Error: " << e.what() << std::endl;			

			//If rename fails copy file and catch any exceptions 
			try {
				copy_file(itr->OriginalFilePath, itr->SortedFilePath);
			}

			catch (filesystem_error& e) {
				std::cerr << "Error: " << e.what() << std::endl;
				return false;
			}

			//Delete existing file at old path and catch any exceptions
			try {
				remove(itr->OriginalFilePath);
			}

			catch (filesystem_error& e) {
				std::cerr << "Error: " << e.what() << std::endl;
				return false;
			}		
		}

		//Record all of the sorted files and their directory in a log.		 
		outputfile << itr->SortedFilePath.filename() << std::endl;		
	}	

	outputfile.close();

	return true;
}

std::string FolderNameMake(std::string FileName) {

	//Initialize the front of the Folder name by taking out the file name and any underscores

	std::string::iterator itr = FileName.begin();
	std::string::iterator itr2;

	if (*itr == '[') {
		itr = std::find_if(find(FileName.begin(), FileName.end(), ']'), FileName.end(), IsAlpha);
		FileName = std::string(itr, FileName.end());
	}
	
	for (std::string::iterator i = FileName.begin(); i != FileName.end(); ++i) {
		if (*i == '_') {
			*i = ' ';
		}
	}
	
	//Initialize the end of the Folder name by finding either a square bracket or a bracket.
	itr = find(FileName.begin(), FileName.end(), '[');
	itr2 = find(FileName.begin(), FileName.end(), '(');

	if (itr < itr2)	{
		while (isalnum(*(itr-1)) == 0 && *(itr-1) != '!') { //Shrinking the end down just in case it has spaces
			--itr;
		}
		FileName = std::string(FileName.begin(), itr);
	}
	else if (itr2 < itr) {
		while (isalnum(*(itr2-1)) == 0 && *(itr2 - 1) != '!' && *(itr2 - 1) != '~') {
			--itr2;
		}
		FileName = std::string(FileName.begin(), itr2);
	}
	
	//Look for a hyphen 	
	if (find(FileName.begin(), FileName.end(), '-') == FileName.end()) {
	//No hypen, go look for a number and hope that it is an episode number or else this is not going to work...

		if (find_if(FileName.begin(), FileName.end(), IsDigit) != FileName.end()) {
			itr = find_if(FileName.begin(), FileName.end(), IsDigit);
			while (isalnum(*(itr-1)) == 0 && *(itr - 1) != '!' && *(itr - 1) != '~') {
				--itr;
			}
			FileName = std::string(FileName.begin(), itr);
		}

	}
	
	//Ok we found a hyphen, check that actually delimits an episode number by checking if there are numbers in the three positions afterward, if not, look for the next hyphen. 
	else {
		itr = FileName.begin();
		while (find(itr, FileName.end(), '-') != FileName.end()) {

			itr = find(itr, FileName.end(), '-');

			if (find_if(itr, itr + 3, IsDigit) != (itr + 3)) {
				while (isalnum(*(itr-1)) == 0 && *(itr-1) != '!') { //Shrink the end down in case of spaces
					--itr;
				}

				FileName = std::string(FileName.begin(), itr);
				break;
			}
			else {
				itr++;
			}
		}
	}


	return FileName;
}

bool IsAlpha(char a) {
	return (std::isalpha(a));
}

bool IsDigit(char a) {
	return (std::isdigit(a));
}

bool metadatacollector(path SortDestinationDirectory) {
	std::string UserName;
	std::string Password;
	std::string MetadataPath;

	std::cout << "Enter your MAL username: " << std::endl;
	std::cin >> UserName;
	std::cout << "Enter your MAL password: " << std::endl;
	std::cin >> Password;

	//Go through the target sort directory. Locate the info.xml in each folder. Get from MAL if the file doesn't exist or it is over 30 days old.
	for (directory_iterator itr(SortDestinationDirectory); itr != directory_iterator(); ++itr) {
		
		if (is_directory(itr->path()) == true) {
			MetadataPath = itr->path().string() + "info.xml";
			
			if (exists(MetadataPath)) {
				if (time(0) - last_write_time(MetadataPath) >= 3000000) {
					return true;
				}
				else {
					MALApiGET(UserName, Password, itr->path().filename().string(), itr->path().string());
				}
			}
			else {
				MALApiGET(UserName, Password, itr->path().filename().string(), itr->path().string());
			}
		
		}		
	}
	return true;
}


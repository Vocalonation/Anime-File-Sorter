#include <iostream>
#include <boost/filesystem.hpp>
#include <iterator>
#include <string>
#include <fstream>
#include <vector>

using namespace boost::filesystem;

struct SortFiles {
	boost::filesystem::path FilePath;
	std::string SortFolderName;
};

int filepicker(boost::filesystem::path SortTargetDirectory, std::vector<SortFiles> & mkvvector, boost::filesystem::path SortDestinationDirectory);
void directorymaker(std::vector <SortFiles> & mkvvector, boost::filesystem::path SortDestinationDirectory, std::vector<SortFiles> & mkvvectordirectory);
bool filemover (std::vector<SortFiles> mkvvector, std::vector<SortFiles> mkvvectordirectory,boost::filesystem::path SortDestinationDirectory);

int main (int argc, char * argv[]) {

//Initialize input commands (using vector for target path and sort destination)	
	std::vector<std::string> commandline;
	commandline.assign(argv+1, argv + argc);
	path SortTargetDirectory;
	path SortDestinationDirectory;
	int FileCounter;

	if (argc == 1) { //For scrubs
		std::cout << "Correct usage is -t for the path to folder to be sorted and -d for the path to the save destination folder." << std::endl;
		std::cout << "Enclose your file path in " " if your has spaces." << std::endl;
		return 0;
	}

	if (commandline[0].compare("-help") == 0) {
		std::cout << "Correct usage is -t for the path to folder to be sorted and -d for the path to the save destination folder." << std::endl;
		std::cout << "Enclose your file path in " " if your has spaces." << std::endl;
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

			else {
				std::cout << "Incorrect arguments. Please see -help.";
			}
	}

	//Path verification

	if (is_directory(SortTargetDirectory) == false) {
		std::cout << "Target directory(" << SortTargetDirectory <<") is an invalid directory." << std::endl;
		return 0;
	}
	else {
		std::cout << SortTargetDirectory << "is a valid directory." << std::endl;
	}

	if (is_directory(SortDestinationDirectory) == false) {
		std::cout << "Destination directory(" << SortDestinationDirectory <<") is an invalid directory." << std::endl;
		return 0;
	}
	else {
		std::cout << SortDestinationDirectory << "is a valid directory." << std::endl;
	}

	//Grab the .mkv files from a directory and write them into a log file
	std::vector<SortFiles> mkvvector, mkvvectordirectory;
	
	FileCounter = filepicker(SortTargetDirectory, mkvvector, SortDestinationDirectory);	
	if (FileCounter <= 0) {
		std::cout << "No files to sort in target directory." << std::endl;
		exit(0);
	}
	std::cout << "Sorting " << FileCounter << " files..." << std::endl;


	//Make the folders based on the file name
	directorymaker(mkvvector,SortDestinationDirectory,mkvvectordirectory);

	std::cout << "Debug flag" << std::endl;

	//Place the files in the corresponding directory. End the program in the case of an exception.
	if (filemover(mkvvector, mkvvectordirectory,SortDestinationDirectory) == false) return 1;

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
	SortFiles mkvfile;	

	//Save 
	for (directory_iterator itr(SortTargetDirectory); itr!= directory_iterator(); ++itr) {
		if(itr->path().extension() == ".mkv") {
			mkvfile.FilePath = itr->path();
			mkvvector.push_back(mkvfile);
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
		outputfile << itr->FilePath << std::endl;
	}
	outputfile.close();

	return FileCounter;
}
	

void directorymaker(std::vector <SortFiles> & mkvvector, boost::filesystem::path SortDestinationDirectory, std::vector<SortFiles> & mkvvectordirectory) {
	
	std::string FileName, FilePath, FolderName, FolderPath;	
	path Foldernameworkaround;
	SortFiles DestinationFiles;


	for (std::vector<SortFiles>::iterator itr = mkvvector.begin(); itr != mkvvector.end(); ++itr) {
		std::size_t found;
		std::size_t found2;

		//Create the folder name by pruning out underscores/fansub group name/chksum/resolution no./episode/etc. from the file name and save it in both mkvvector and mkvvectordirectory

		Foldernameworkaround = (*itr).FilePath.stem();		
		FileName = Foldernameworkaround.string();
		found2 = FileName.find_first_of(']');
		FileName = FileName.substr(found2+2);		

		for (std::string::iterator i = FileName.begin(); i != FileName.end(); ++i) {
			if (*i == '_') {
				*i = ' ';
			}
		}

		if (FileName.find_last_of('-') != std::string::npos){
			found = FileName.find_last_of('-');
			FolderName = FileName.substr(0,(found-1));
			
			//Saving to mkvvector 
			itr->SortFolderName = FolderName;
			
			//Saving to temporary struct which will be added to mkvvectordirectory
			DestinationFiles.SortFolderName = FolderName;			
		}

		else if (FileName.find_first_of('[') != std::string::npos) {
		 	found = FileName.find_first_of('[');
		 	FolderName = FileName.substr(0,(found-1));		

			//Saving to mkvvector 
			itr->SortFolderName = FolderName;

			//Saving to temporary struct which will be added to mkvvectordirectory
			DestinationFiles.SortFolderName = FolderName;					
		}		
		
		else if (FileName.find_first_of('(') != std::string::npos) {
			found = FileName.find_first_of('(');
			FolderName = FileName.substr(0, (found - 1));

			//Saving to mkvvector 
			itr->SortFolderName = FolderName;

			//Saving to temporary struct which will be added to mkvvectordirectory
			DestinationFiles.SortFolderName = FolderName;			
		}
		
		//Skip the file, if the file name somehow creates problems.
		else {
			continue;
		}


		
		//Build folder path
		FolderPath = SortDestinationDirectory.string() + "\\" + FolderName;
		path p(FolderPath);		
		
		//Build File path
		FilePath = FolderPath + "\\" + itr->FilePath.filename().string();
		
		//Save the paths of all of the created folders and/or existing folders in mkvvectordirectory
		DestinationFiles.FilePath = FilePath;
		mkvvectordirectory.push_back(DestinationFiles);
		
		//Create the folders if they don't exist
		try {
			create_directory(p);
		}

		catch (filesystem_error& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			exit(0);
		}
	}	

	return;
}

bool filemover(std::vector<SortFiles> mkvvector, std::vector<SortFiles> mkvvectordirectory, boost::filesystem::path SortDestinationDirectory) {

	std::ofstream outputfile;
	std::string DestinationPath;

	DestinationPath = SortDestinationDirectory.string() + "\\Sorted Files.txt";
	outputfile.open(DestinationPath,std::ios::out | std::ios::trunc);

	if(outputfile.bad() == true) {
		std::cout << "Log file write failed." << std::endl;
		return 0;
	}

	//Match path in mkvvector with the correct folder path contained in mkvvectordirectory
 	for (std::vector<SortFiles>::const_iterator itr = mkvvector.begin(); itr != mkvvector.end(); ++itr) {
 		
 		outputfile << "Directory: " << itr->SortFolderName << std::endl;

 		for (std::vector<SortFiles>::const_iterator itr2 = mkvvectordirectory.begin(); itr2 != mkvvectordirectory.end(); ++itr2) {

 			if (itr->SortFolderName == itr2->SortFolderName) {
				std::cout << "Now sorting: " << itr->FilePath <<std::endl;

				//Move the file to the new path and catch any exceptions
				try {
					rename(itr->FilePath, itr2->FilePath);
				}

				catch (filesystem_error& e) {
					std::cerr << "Error: " << e.what() << std::endl;
					
					//If rename fails copy file and catch any exceptions 
					try {
						copy_file(itr->FilePath, itr2->FilePath);
					}

					catch (filesystem_error& e) {
						std::cerr << "Error: " << e.what() << std::endl;
						outputfile.close();
						return false;
					}

					//Record all of the sorted files and their directory in a log.
					outputfile << itr->FilePath.filename() << std::endl;
					outputfile << std::endl;

					//Delete existing file at old path and catch any exceptions
					try {
						remove(itr->FilePath);
					}

					catch (filesystem_error& e) {
						std::cerr << "Error: " << e.what() << std::endl;
						outputfile.close();
						return false;					
					}

					//Record all of the sorted files and their directory in a log.
					outputfile << itr->FilePath.filename() << std::endl;
					outputfile << std::endl;
				}				
			
				if (itr != mkvvector.end()) ++itr;
			}			
 		}
 	}

 	outputfile.close();
	return true;

 }


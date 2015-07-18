#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include "AnimeData.h"

#include <curl.h>

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

int MALApiGET(std::string UserName, std::string Password, std::string AnimeName, std::string Path)
{
	CURL *curl_handle;

	//Build info.xml path
	std::string FilePath;

	FilePath = Path + '\\' + "info.xml";

	static const char *pagefilename = FilePath.c_str();
	FILE *pagefile;

	//Initialize
	curl_global_init(CURL_GLOBAL_ALL);

	//Init the curl session
	curl_handle = curl_easy_init();

	//Construct url
	for (std::string::iterator itr = AnimeName.begin(); itr != AnimeName.end(); ++itr) {
		if (*itr == ' ') *itr = '+';
	}

	std::string query = MALQUERY + AnimeName; //Defined in AnimeData.h
	char URL[100];
	strcpy(URL, query.c_str());

	std::cout << query << std::endl;
	curl_easy_setopt(curl_handle, CURLOPT_URL, URL);

	//Switch on full protocol/debug output while testing
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

	//Send all data to this function
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

	//Disable progress meter, set to 0L to enable and disable debug output
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	//Authenticate
	char authentication[100];
	strcpy(authentication, (UserName + ":" + Password).c_str());

	curl_easy_setopt(curl_handle, CURLOPT_USERPWD, authentication);

	//Authentication protocol
	curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

	/* example.com is redirected, so we tell libcurl to follow redirection */
	curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

	// open the file 	
	pagefile = fopen(pagefilename, "wb");
	if (pagefile) {

		// write the page body to this file handle 
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

		// get it! 
		curl_easy_perform(curl_handle);

		// close the header file 
		fclose(pagefile);

		//Clean up curl stuff
		curl_easy_cleanup(curl_handle);



		return 0;
	}

	else {
		//Clean up curl stuff
		curl_easy_cleanup(curl_handle);

		// close the header file 
		fclose(pagefile);


		return 1;
	}
}
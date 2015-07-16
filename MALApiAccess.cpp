#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <curl.h>



static size_t write_data(void *ptr, size_t size, size_t nmemb, void* stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;

}


int MALApiGET(std::string UserName, std::string Password)
{
	CURL *curl_handle;
	static const char *pagefilename = "page.out";
	FILE *pagefile;

	curl_global_init(CURL_GLOBAL_ALL);

	//Init the curl session
	curl_handle = curl_easy_init();

	//Specify the URL to get
	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://myanimelist.net/api/anime/search.xml?q=Hibike!+Euphonium");

	//Switch on full protocol/debug output while testing
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

	//Send all data to this function
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

	//Disable progress meter, set to 0L to enable and disable debug output
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	//Authenticate
	curl_easy_setopt(curl_handle, CURLOPT_USERPWD, "Vocalonation:Miku3939!");

	//Authentication protocol
	curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);

	//Open the file
	pagefile = fopen(pagefilename, "wb");
	if (pagefile) {
		//Write the page body to this file handle
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

		//get it

		curl_easy_perform(curl_handle);

		//Close the header file
		fclose(pagefile);
	}

	//Clean up curl stuff
	curl_easy_cleanup(curl_handle);

	return 0;
}
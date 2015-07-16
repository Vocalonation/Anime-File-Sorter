#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include <curl.h>

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}


int MALApiGET(std::string UserName, std::string Password, std::string AnimeName)
{
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	//Construct MAL URL

	curl_global_init(CURL_GLOBAL_ALL);

	//Init the curl session
	curl_handle = curl_easy_init();

	//Specify the URL to get
	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://myanimelist.net/api/anime/search.xml?q=Hibike!+Euphonium");

	//Switch on full protocol/debug output while testing
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

	//Send all data to this function
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

	//Disable progress meter, set to 0L to enable and disable debug output
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	/* some servers don't like requests that are made without a user-agent
	field, so we provide one */
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	//Authenticate
	curl_easy_setopt(curl_handle, CURLOPT_USERPWD, "Vocalonation:Miku3939!");

	//Authentication protocol
	curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);


	/* check for errors */
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	}


	else {
		//Now what am I going to do with this...

		//Parse the string

		//Return some data structure with the metadata I guess? and write the metadata separately
	}

	//Clean up curl stuff
	curl_easy_cleanup(curl_handle);

	free(chunk.memory);

	return 0;
}
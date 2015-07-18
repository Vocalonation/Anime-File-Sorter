/*************************************************************/
//Include guard
#ifndef __MALAPIAccess_H_INCLUDED__
#define __MALAPIAccess_H_INCLUDED__

//Include dependencies
#include <string>


//Functions

static size_t write_data(void *ptr, size_t size, size_t nmemb, void* stream);
int MALApiGET(std::string UserName, std::string Password, std::string AnimeName, std::string Path);




#endif




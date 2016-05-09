#ifndef DOWNLOADER_H_1234
#define DOWNLOADER_H_1234
#include <cstring>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <cstdlib>
using namespace std;



/*			FTPFILE STRUCTURE
*
* 	name:				FtpFile
* 	description:	It is used by the 'download()'.
* 	fields:			filename	-	The name of the file.
*						stream	-	The output stream where the data are saved.
*/
struct FtpFile
{
  char *filename;
  FILE *stream;
};



size_t my_fwrite(void*, size_t, size_t, void*);
int download(string, string);
int checkFile(string);
#endif

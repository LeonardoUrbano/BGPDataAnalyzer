#include "Downloader.h"



/*			MYFWRITE FUNCTION
*
* 	name:				my_fwrite
* 	description:	It writes the bytes contained in the buffer into the output stream, i.e. the
*						output file. It is called automatically by curl.
* 	parameters:		buffer	-	The buffer contained the bytes to write.
*						size		-	The data unit expressed in bytes.
*						nmemb		-	The number of bytes to write.
*						stream	-	The output stream where the bytes have to be written.
* 	returned:		The function returns the number of bytes successfully written or -1 in case
*						of errors.
*/
size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
	// initialize the output stream	
	struct FtpFile *out=(struct FtpFile *)stream;

	if(out && !out->stream)
	{
		// open file for writing
		out->stream=fopen(out->filename, "wb");
		if(!out->stream)
			return -1; // failure, can't open file to write
	}
	
	return fwrite(buffer, size, nmemb, out->stream);
}



/*			DOWNLOAD FUNCTION
*
* 	name:				download
* 	description:	It downloads the file at the specified url in the specified file.
* 	parameters:		url			-	The address of the resource to download from the Internet.
*						filePath		-	The file where the data must be saved.
* 	returned:		If the resource is successfully downloaded, the function returns 0.
*						It returns -1 if something goes wrong.
*/
int download(string url, string filePath)
{	
	int ret = 0;
	
	// downloading
	struct FtpFile ftpfile;
	ftpfile.filename = (char*)filePath.c_str();
	ftpfile.stream = NULL;
		// set up ccurl
		CURL *curl_handle;
		CURLcode res;
		// init the curl session
  		curl_handle = curl_easy_init();
		// specify URL to get
		curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
		// send all data to this function
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, my_fwrite);
		// we pass our 'chunk' struct to the callback function
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &ftpfile);
  		// some servers don't like requests that are made without a user-agent
		// field, so we provide one
  		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		// get it
		res = curl_easy_perform(curl_handle);
		// check for errors
		if(res != CURLE_OK)
		{			
			ret = -1;
			goto exit_getBgp;
		}

	if(ftpfile.stream)
   	fclose(ftpfile.stream);

exit_getBgp:	// cleanup curl stuff
 					curl_easy_cleanup(curl_handle);
					return ret;
}



/*			CHECKFILE FUNCTION
*
* 	name:				checkFile
* 	description:	It checks if the downloaded file is valid or not.In particular, it isn't valid if
*						it is impossible to locate the resource in the Internet.
* 	parameters:		filePath	-	The file that has to be checked.
* 	returned:		It returns 0 if the file is valid, 1 otherwise.
*/
int checkFile(string filePath)
{
	// vars	
	int k, y=0;
	string line;
	// error strings
	string error="404 Not Found";
	string error1="Error 404 - Page not found!";

  	ifstream myfile (filePath.c_str());
  	if (myfile.is_open())
	{
    		while ( getline (myfile,line) ){
			k=line.find(error);
			y=line.find(error1);
      			if(k>=0 || y>=0)
				return 1;
		}
    	myfile.close();
  	}
	else
		return 1;
	return 0;
}

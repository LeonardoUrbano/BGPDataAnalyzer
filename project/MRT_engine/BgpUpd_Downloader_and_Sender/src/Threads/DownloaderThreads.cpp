#include "DownloaderThreads.h"



/*			GETBGPUPDNAME FUNCTION
*
* 	name:				getBgpUpdName
* 	description:	Starting from a given url, the function splits the url string and
*						returns the name of the BGP update.
* 	parameters:		url	-	The BGP update url that has to be splitted.
* 	returned:		It returns the name of the BGP update.
*/
string getBgpUpdName(string url)
{
	size_t i = url.find_last_of("/");
	return url.substr(i + 1);
}



/*			DOWNLOADWITHOUTNORMALIZATION FUNCTION
*
* 	name:				download_without_normalization
* 	description:	It downloads a BGP update from the route collector 'rc' without computing
*						any normalization on the timestamp. This is used whenever a BGP update cannot
*						be located in the Internet since it has been loaded with a given delay with
*						respect to the nominal value.
* 	parameters:		baseDir	-	The main folder where the file has to be saved.
*						filePath	-	The file where the file has to be saved.
*						rc			-	The route collector.
* 	returned:		-
*/
void download_without_normalization(string baseDir, string filePath, RouteCollector* rc)
{	
	remove(filePath.c_str());
	for(int k=0; k<rc->timer-1; k++)
	{
		rc->nextUrl(false);
		filePath=baseDir + rc->rcName + getBgpUpdName(rc->lastUrl);
		download(rc->lastUrl, filePath);
		if(checkFile(filePath)==0)
		{
			rc->valid=true;
			break;
		}
		remove(filePath.c_str());
	}
}



/*			DOWNLOADBGPFUNCTION FUNCTION
*
* 	name:				downloadBgpFunction
* 	description:	It is the body of the download thread. It does the initial burst, then it waits
*						until all threads finish the burst and finally it tries to downlaod the next BGP updates.
* 	parameters:		ptr	-	It is an index which allows to distinguish Isolario, RouteViews or Ris.
* 	returned:		-
*/
void* downloadBgpFunction(void* ptr)
{	
	// vars	
	time_t rawtime;
  	struct tm * ptm;
	RouteCollector** rc;
	string tmpStr = string();
	string baseDir = string();
	string filePath = string();
	int N_RC;

	// get the UTC time
	time ( &rawtime );
 	ptm = gmtime ( &rawtime );	
	
	// select the correct case, create the right route collector and folders
	switch(*(int*)ptr)
	{
		case 0:
			N_RC = N_RC_RV;
			rc=new RouteCollector* [N_RC];
			baseDir = BaseDirRV;
			for(int i=0; i<N_RC_RV; i++){
				rc[i] = new RouteViewRC(RVIEWS[i], 15, ptm);
				tmpStr = baseDir + rc[i]->rcName;
				mkdir(tmpStr.c_str(), S_IRWXU);
			}
			break;
			
		case 1:
			N_RC = N_RC_RIS;
			rc=new RouteCollector* [N_RC];
			baseDir = BaseDirRis;
			for(int i=0; i<N_RC_RIS; i++){
				rc[i] = new RisRC(RIS[i], 5, ptm);
				tmpStr = baseDir + rc[i]->rcName;				
				mkdir(tmpStr.c_str(), S_IRWXU);
			}
			break;

		case 2:
			N_RC = N_RC_ISO;
			rc=new RouteCollector* [N_RC];
			baseDir = BaseDirIso;
			for(int i=0; i<N_RC_ISO; i++){
				rc[i] = new IsolarioRC(ISO[i], 5, ptm);
				tmpStr = baseDir + rc[i]->rcName;
				mkdir(tmpStr.c_str(), S_IRWXU);
			}
			break;
	}


	// BURST START
	// It gets the last available update, then goes back until the oldest file that must
	// be downloaded is reached and, finally, it moves forward until the newest file has
	// been downloaded from the Internet.
	for(int i=0; i<N_RC; i++)
	{
		// get the last update for the current repository of the route collector		
		while(true)
		{
			filePath=baseDir + rc[i]->rcName + getBgpUpdName(rc[i]->lastUrl);
			download(rc[i]->lastUrl, filePath);
			if(checkFile(filePath)==0)
				break;
			// since it's the initial burst, the file MUST EXIST
			// if the update cannot be found, try without normalization on the filename
			download_without_normalization(baseDir, filePath, rc[i]);
			if(rc[i]->valid)
				break;
			rc[i]->precUrl();
		}
		// go back until the oldest update to be downloaded
		for(int j=0; j<=(first_bgpupd_since/rc[i]->timer)-2;  j++)
			rc[i]->precUrl();
		// go forward until the newest update to be downloaded
		for(int j=0; j<=(first_bgpupd_since/rc[i]->timer)-2;  j++)
		{
			filePath=baseDir + rc[i]->rcName + getBgpUpdName(rc[i]->lastUrl);
			download(rc[i]->lastUrl, filePath);
			if(checkFile(filePath)!=0)
				download_without_normalization(baseDir, filePath, rc[i]);
			rc[i]->nextUrl();
		}	
		
	}
	// BURST COMPLETE

	
	// SYNCH	
	// signal the 'FlowCreateThread' that is blocked
	pthread_mutex_lock( &count_mutex );
		threadCount++;
		pthread_cond_signal( &condition_var );
	pthread_mutex_unlock( &count_mutex );


	// STABLE BEHAVIOUR
	// download the most recent BGP updates belonging to the route collector
	while(true)
	{
		// try to download the BGP updates for all the repositories		
		for(int i=0; i<N_RC; i++)
		{		
			// try to download the next BGP update for the current repository
			if(rc[i]->valid)
				rc[i]->nextUrl();
			filePath=baseDir + rc[i]->rcName + getBgpUpdName(rc[i]->lastUrl);
			download(rc[i]->lastUrl, filePath);
			// check the downloaded file
			if(checkFile(filePath)==0)
			{
				rc[i]->valid=true;
				// the file is valid, so signal the 'FlowCreate' thread that could be blocked				
				pthread_mutex_lock( &count_mutex );
					pthread_cond_signal( &condition_var );
				pthread_mutex_unlock( &count_mutex );	
			}
			else
			{
				rc[i]->valid=false;
				download_without_normalization(baseDir, filePath, rc[i]);
				if(rc[i]->valid==false)
				   rc[i]->getUrl();
			}
		}
		sleep(60);
	}
}

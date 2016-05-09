#include "BgpUpd_Downloader_and_Sender.h"



// global variables used for synchronization between 'downloadThread' and 'flowCreateThread'
pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_var   = PTHREAD_COND_INITIALIZER;
int threadCount = 0;	



/*			MAIN FUNCTION
*
* 	name:				main
* 	description:	It performs the following operation:
*						1) it creates the directories will contain the repositories,
*						2) it creates three threads that will compute the downloads,
*						3) it creates a thread which will make a single flow from the whole set.
* 	parameters:		-
* 	returned:		-
*/
int main()
{	
	// vars
	pthread_t repThreads[N_REPOSITORY];
	pthread_t flowCreateThread;
	int iret[N_REPOSITORY];
	int id[3];

	//init curl
	curl_global_init(CURL_GLOBAL_ALL);
	// create main folder
	mkdir("../REP", S_IRWXU);
	mkdir("../REP/RV", S_IRWXU);
	mkdir("../REP/RIS", S_IRWXU);
	mkdir("../REP/ISO", S_IRWXU);
	// create three downloader threads, one for each route collector
	for(int i=0; i<N_REPOSITORY; i++)
	{
		id[i] = i;		
		iret[i] = pthread_create(&repThreads[i], NULL, downloadBgpFunction, (void*)&id[i]);
		if(iret[i])
		{
			fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[i]);
         exit(EXIT_FAILURE);
		}
	}
	// create the 'flowCreateThread'
	int r = pthread_create(&flowCreateThread, NULL, flow_create, NULL);
	if(r)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n", r);
		exit(EXIT_FAILURE);
	}
	// join all the threads
	for(int i=0; i<N_REPOSITORY; i++)	
		pthread_join(repThreads[i], NULL);
	pthread_join(flowCreateThread, NULL);
	// clean curl
	curl_global_cleanup();

	exit(EXIT_SUCCESS);
}


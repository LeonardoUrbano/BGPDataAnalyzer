#include "FlowCreateThread.h"
// Global 'Geolocator_core' object. It is used by the 'SenderThread'.
// Since it takes some minutes to load the Geolocator, the latter is created here.
Geolocator_core* geo;



/*			LSCMD FUNCTION
*
* 	name:				ls_cmd
* 	description:	It execute the 'ls *.?z* | sort' command.
*						In particular, it looks for '.bz2'/'.gz' files, sorts them and,
*						finally, save the name of the file in the 'file_name' variable which
*						is passed as input to the function.
* 	parameters:		file_name	-	The variable that will contain the name of the last BGP
*											update in the folder.
*								index	-	The index of the repository.
* 	returned:		If the folder isn't empty, it stores the name of the last BGP update in the folder
*						in the 'file_name' variable and returns 0. It returns -1, otherwise.
*/
int ls_cmd(string& file_name, int index)
{
	// vars
	string cmd = string("ls ");
	string tmp = string();

	// prepare the command	
	if (index<=17)
		cmd += BaseDirRV + RVIEWS[index];
	else
	{
		if(index>30)
			cmd += BaseDirIso + ISO[index-N_RC_RV-N_RC_RIS];
		else
			cmd += BaseDirRis + RIS[index-N_RC_RV];
	}
	cmd += "*.?z* | sort";

	// execute the command
	tmp = exe(cmd);

	// check the result
	if(tmp != "")
	{
		int pos = tmp.find_first_of("\n");
		file_name = tmp.substr(0,pos);
		return 0;
	}

	return -1;
}



/*			FLOWCREATE FUNCTION
*
* 	name:				flow_create
* 	description:	It is the body of the 'FlowCreateThread'.
*						First of all, it loads the geolocator and creates the 'SenderThread', then
*						it creates a unique flow - from the whole set of repositories -  that will be
*						processed by the 'SenderThread'.
*						The unique out flow is implemented by means of a 'SharedQueue' which manages the
*						synchronization between different threads by itself.
* 	parameters:		ptr	-	It is always equal to 'NULL'.
* 	returned:		NULL
*/
void* flow_create(void * ptr)
{
	// Load Geolocator
	Log geol("../../log/geolocator.txt");	
	cout<<"Loading Geolocator.. "<<endl;
	geo = new Geolocator_core(&geol);
	cout<<"done"<<endl;		
	// Create flows
	RcFlow flows[N_RC_RV + N_RC_RIS + N_RC_ISO];
	string file_name = string();
	int index;
	uint32_t min_timestamp, global_timestamp = 0xFFFFFFFF;
	Log l("../../log/flow_create.txt");
	// Create sender thread
	pthread_t senderThread;
	int r = pthread_create(&senderThread, NULL, send, NULL);
	if(r)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n", r);
		exit(EXIT_FAILURE);
	}


	// BURST SYNCH
	// wait until all the downloader threads finish the initial burst
	while(true)
	{
		 pthread_mutex_lock( &count_mutex );
			if(threadCount == N_REPOSITORY)
				break;
      	pthread_cond_wait( &condition_var, &count_mutex );
      pthread_mutex_unlock( &count_mutex );
	}
	

	// INIT FLOW
	// initialize each single flow
	for(int i=0; i<N_RC_RV + N_RC_RIS + N_RC_ISO; i++)
	{
		// get the 1st BGP update to read		
		ls_cmd(file_name, i);
		// parse the file and delete it
		flows[i].m = new MDR(file_name, "", file_name, &l);
		remove(file_name.c_str());
		// push all the entries in the single flow		
		flows[i].m->run_and_push_in_queue(&flows[i].sq);
		// extract the 1st entry and initialize 'firstEntry'
		if(flows[i].sq.get_sem_value() > 0)
			flows[i].firstEntry = flows[i].sq.pop();
	}
	// START
	// begin the loop
	cout<<endl<<"START"<<endl<<endl;
	while(1)
	{
		//  find the minimum timestamp
		min_timestamp = 0xFFFFFFFF;	
		for(int i=0; i<N_RC_RV + N_RC_RIS + N_RC_ISO; i++)
		{	
			// the single flow is empty, jump to the refill 
			if(flows[i].firstEntry == NULL)
			{
				index = i;				
				goto label;
			}
			// find the index
			if(flows[i].firstEntry->timestamp == global_timestamp)
			{
				min_timestamp = flows[i].firstEntry->timestamp;
				index = i;
				break;
			}	
			if(flows[i].firstEntry->timestamp < min_timestamp){
					min_timestamp = flows[i].firstEntry->timestamp;
					index = i;
			}
		}
		global_timestamp = min_timestamp;
		
		// push the entry in the ouput flow
		outFlow.push(flows[index].firstEntry);
		// if there are no clients connected, delete the oldest entries
		pthread_mutex_lock(&client_mutex);
			if(websockets.empty() && outFlow.get_sem_value() >= 1000000)
			{
				for(int j=0; j<100000; j++)
					outFlow.pop();
			}
		pthread_mutex_unlock(&client_mutex);

		// load the new file - if it's the case - and extract a new entry
label:if(flows[index].sq.get_sem_value() > 0)
		{
			flows[index].firstEntry = flows[index].sq.pop();
		}
		// the single flow is empty, load the next file
		else
		{
			// get the next update from the disk or wait for its download
			while((ls_cmd(file_name, index)) == -1)
			{
				pthread_mutex_lock( &count_mutex );					
					pthread_cond_wait( &condition_var, &count_mutex );
				pthread_mutex_unlock( &count_mutex );
			}
			// reset the variables
			delete flows[index].m;
			flows[index].firstEntry = NULL;
			// parse the file and delete it
			flows[index].m = new MDR(file_name, "", file_name, &l);
			remove(file_name.c_str());
			// push all the entries in the single flow
			flows[index].m->run_and_push_in_queue(&flows[index].sq);
			// extract the 1st entry and initialize 'firstEntry'
			if(flows[index].sq.get_sem_value() > 0)
				flows[index].firstEntry = flows[index].sq.pop();
		}
	}
	
	return ptr;
}

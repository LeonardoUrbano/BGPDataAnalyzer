#ifndef FLOW_CREATE_THREAD_H_1234
#define FLOW_CREATE_THREAD_H_1234
#include "common.h"
#include "../../../Common_files/Utils/Commands.h"
#include "SenderThread.h"
#include <map>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>



/*			RCFLOW STRUCTURE
*
* 	name:				RcFlow
* 	description:	It is used by the 'flow_create()'.
* 	fields:			m				-	The MDR file reader.
*						sq				-	The shared queue where all the entries of the MDR file are pushed.
*						firstEntry	-	The last entry extracted.
*/
struct RcFlow{
	MDR* m;
	SharedQueue<BGP_entry*> sq;
	BGP_entry* firstEntry = NULL;
};



//	Variables for the synchronization with the 'SenderThread'
extern SharedQueue<BGP_entry*> outFlow;
extern pthread_mutex_t client_mutex;
extern map<string, websocketpp::connection_hdl> websockets;
void* flow_create(void *);
#endif

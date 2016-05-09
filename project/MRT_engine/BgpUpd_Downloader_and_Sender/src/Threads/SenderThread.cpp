#include "SenderThread.h"

// shared variables among threads
SharedQueue<BGP_entry*> outFlow;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
// global variables
BGP_entry* entry = NULL;
map<string, mapItem> websockets;
string resp_msg = string("");
websocketpp::server<websocketpp::config::asio> server;
websocketpp::lib::error_code ec;
uint32_t timest = 0;
LogStream ls;
ostream os(&ls);



/*			PREPAREMSG FUNCTION
*
* 	name:				prepare_msg
* 	description:	It extracts the next entry from the 'sharedQueue', prepares the message and
*						stores the latter in the 'resp_msg' variable. The message format is the following:
*						| IP_from_and_geo | WITHDRAWN_ROUTES | ANNOUNCED_ROUTE_and_geo | AS_PATH(A)/AS_FROM(W) |
* 	parameters:		-
* 	returned:		-
*/
void prepare_msg()
{	
	string ASpath = string();
	string last_insert = string("");	
	string IP_from = string();
	vector<string> ANN_route, WTH_route;
	vector<string> v_tmp;

	// get the next entry
	if(entry != NULL)
		delete entry;
	entry = outFlow.pop();
	while((entry->subtype != BGP4MP_MESSAGE_AS4) && (entry->subtype != BGP4MP_MESSAGE))
	{
		delete entry;
		entry = outFlow.pop();
	}
	
	// extract information
	BGP_update tmp(entry->BGP_packet->pkt, entry->asn32bit);
		// get IPfrom and its geolocalization	
		resp_msg = entry->IPfrom + "-" + geo->geolocate(entry->IPfrom) + ";";
		// get withdrawn routes and their geolocalization
		if(tmp.get_every_WITHDRAWN_ROUTE() != "")
		{
			WTH_route = tmp.get_every_WITHDRAWN_ROUTE_vec();
			for(unsigned int i=0; i<WTH_route.size(); i++)
				resp_msg += WTH_route[i] + "-" + geo->geolocate(WTH_route[i]) + " ";
			resp_msg = resp_msg.substr(0, resp_msg.size() - 1);
		}
		resp_msg += ";";
		// get announced routes and their geolocalization	
		if(tmp.get_every_ANNOUNCED_ROUTE() != "")
		{
			ANN_route = tmp.get_every_ANNOUNCED_ROUTE_vec();
			for(unsigned int i=0; i<ANN_route.size(); i++)
				resp_msg += ANN_route[i] + "-" + geo->geolocate(ANN_route[i]) + " ";
			resp_msg = resp_msg.substr(0, resp_msg.size() - 1);
		}
		resp_msg += ";";
		// get AS path
		if(tmp.has_AS_PATH())
		{
			v_tmp=tmp.get_AS_PATH_vec();	
			for(unsigned int i=0; i<v_tmp.size(); i++){
				if(v_tmp[i] != last_insert){
					last_insert=v_tmp[i];
					ASpath += last_insert + " ";
				}
			}
			resp_msg += ASpath.substr(0, ASpath.size() - 1);	
		}
		else
			resp_msg += to_string(entry->ASfrom);
		resp_msg += ";" + to_string(entry->timestamp); 
		
}



/*			ON_CLOSE FUNCTION
*
* 	name:				on_close
* 	description:	Whenever a client disconnects itself, the function removes all the structures
*						allocated to that client on the server.
* 	parameters:		hdl	-	The connection that fires the 'close' event.
* 	returned:		-
*/
void on_close(websocketpp::connection_hdl hdl)
{
	pthread_mutex_lock(&client_mutex);
		// get the endpoint from the connection, the key of the map
		websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(hdl);
		bool received = websockets[con->get_remote_endpoint()].received;
		// erase the entry from the map
		websockets.erase(con->get_remote_endpoint());
		// check if a send must be done
		if(!received)
		{
			map<string, mapItem>::iterator it;
			for(it=websockets.begin(); it!=websockets.end(); ++it)
				if(!(it->second.received))
					goto exit_onclose;
		
			prepare_msg();
			for(it=websockets.begin(); it!=websockets.end(); ++it)
			{
					server.send(it->second.connection, resp_msg, websocketpp::frame::opcode::text, ec);
					it->second.received = false;
			}
		}
exit_onclose:	pthread_mutex_unlock(&client_mutex);
}



/*			ONOPEN FUNCTION
*
* 	name:				on_open
* 	description:	Whenever a new client connects to the server, the function allocates all
*						the structures needed in order to handle the new client.
* 	parameters:		hdl	-	The connection that fires the 'open' event.
* 	returned:		-
*/
void on_open(websocketpp::connection_hdl hdl)
{
	pthread_mutex_lock(&client_mutex);
		// get the endpoint from the connection, it will be used as key of the map		
		websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(hdl);
		mapItem tmpItem;
		tmpItem.connection = hdl;
		// insert into the map
		tmpItem.received = false;
		websockets.insert(pair<string, mapItem>(con->get_remote_endpoint(), tmpItem));
	pthread_mutex_unlock(&client_mutex);
}



/*			ONMESSAGE FUNCTION
*
* 	name:				on_message
* 	description:	This function is called whenever a client replies with a message.
*						Basically, it checks if all the clients have replied and, if it is
*						the case, the server sends the new entry to all the client; otherwise,
*						it waits until all the clients have replied.
* 	parameters:		hdl	-	The connection from which the message has arrived.
*						msg	-	The reply message. It is never used.
* 	returned:		-
*/
void on_message(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
{
	pthread_mutex_lock(&client_mutex);
		map<string, mapItem>::iterator it;
		// get the endpoint, the key
		websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(hdl);
		string key = con->get_remote_endpoint();
		// set the received variable to TRUE
		websockets[key].received = true;
	
	// check if a send must be done
	for(it=websockets.begin(); it!=websockets.end(); ++it)
		if(!(it->second.received))
			goto exit_onmsg;
	
	// a new send must be done
	// prepare the message
	prepare_msg();
	// send the message to all the clients
	for(it=websockets.begin(); it!=websockets.end(); ++it)
	{
			server.send(it->second.connection, resp_msg, websocketpp::frame::opcode::text, ec);
			it->second.received = false;
	}
exit_onmsg:	pthread_mutex_unlock(&client_mutex);
}



/*			SEND FUNCTION
*
* 	name:				send
* 	description:	It is the body of the sender thread. It initializes the websocket structure and
*						then starts to accept and to serve the requests, indefinetely.
* 	parameters:		ptr	-	It is always 'NULL'.
* 	returned:		It returns always 'NULL'.
*/
void* send(void * ptr)
{
	try
	{
		// Initialising WebSocketServer
    	server.init_asio();
		// Redirect the output into a log file
		server.get_alog().set_ostream(&os);
		server.get_elog().set_ostream(&os);
		// Register the message handlers
		server.set_open_handler(&on_open);
		server.set_message_handler(&on_message);
		server.set_close_handler(&on_close);
		// Listen on port
		int port = 8082;
		server.listen(port);
		// Starting Websocket accept
    	server.start_accept(ec);
		server.run();
	}

	catch(websocketpp::exception const &e)
	{
		// Websocket exception on listen
	}

	// loop
	while(1) {}	
	return ptr;
}

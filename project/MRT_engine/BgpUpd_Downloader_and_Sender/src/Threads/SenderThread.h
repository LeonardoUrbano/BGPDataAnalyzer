#ifndef SENDER_H_1234
#define SENDER_H_1234
#include "../../../Geolocator/src/Geolocator_core.h"
#include "../../../Common_files/utils.h"
#include "../../../Common_files/Log.h"
#include "../../../MRT_data_reader/src/MDR_core.h"
#include "../../../Common_files/BGP/BGP_RIB_snapshot.h"
#include "../../../Common_files/MRT/MRT_BGP4MP_STATE_CHANGE.h"
#include "../../../Common_files/MRT/MRT_BGP4MP_STATE_CHANGE_AS4.h"
#include "../../../Common_files/MRT/MRT_BGP4MP.h"
#include <map>
#include <vector>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <iostream>

extern Geolocator_core* geo;
void* send(void*);



/*			LOGSTREAM CLASS
*
* 	name:				LogStream
* 	description:	It is used by the websocket to redirect the output.
* 	variables:		buffer	-	The output buffer.
* 	methods:			overflow	-	it is used by the websocket.
*/
class LogStream : public streambuf {    
private:
    string buffer;
     
protected:
    int overflow(int ch) override {
        buffer.push_back((char) ch);
        if (ch == '\n') {
            // End of line, write to logging output and clear buffer.
             
            buffer.clear();
        }
         
        return ch;
         
        //  Return traits::eof() for failure.
    }
};



/*			MAPITEM STRUCTURE
*
* 	name:				mapItem
* 	description:	It is used by the 'send()' to handle the connections.
* 	fields:			connection	-	The connection associated to a specific client.
*							received	-	A boolean value which specifies if the client has replied yet.
*/
struct mapItem
{
	websocketpp::connection_hdl connection;
	bool received;
};
#endif

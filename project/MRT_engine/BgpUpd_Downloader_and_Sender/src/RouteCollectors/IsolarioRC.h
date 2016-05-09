#ifndef ISOLARIO_H_1234
#define ISOLARIO_H_1234
#include "RouteCollector.h"

#define ISOLARIO_BASEURL ("https://www.isolario.it/Isolario_MRT_data/") // base url
#define UPD_NAME ("updates.") // update name



/*			ISOLARIORC	CLASS
*
* 	name:				IsolarioRc
* 	description:	It is a derived class from the 'RouteCollector' one.
*						It allows to generate the BGP updates' urls for the Isolario's route collectors.
* 	variables:		-
* 	methods:			getUrl	-	it defines how to generate the urls for the BGP updates coming from the 
*										Isolario's route collectors.
*/
class IsolarioRC : public RouteCollector
{
	public:	IsolarioRC(string, int, tm*);
				virtual void getUrl(bool = true);
};


#endif

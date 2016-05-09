#ifndef RIS_H_1234
#define RIS_H_1234
#include "RouteCollector.h"

#define RIS_BASEURL ("http://data.ris.ripe.net/") // base url
#define UPD_NAME ("updates.") // update name



/*			RISRC	CLASS
*
* 	name:				RisRc
* 	description:	It is a derived class from the 'RouteCollector' one.
*						It allows to generate the BGP updates' urls for the Ris' route collectors.
* 	variables:		-
* 	methods:			getUrl	-	it defines how to generate the urls for the BGP updates coming from the 
*										Ris' route collectors.
*/
class RisRC : public RouteCollector
{
	public:	RisRC(string, int, tm*);
				virtual void getUrl(bool = true);
};


#endif

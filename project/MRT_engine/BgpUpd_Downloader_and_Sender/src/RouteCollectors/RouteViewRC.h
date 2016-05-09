#ifndef ROUTEVIEWRC_H_1234
#define ROUTEVIEWRC_H_1234
#include "RouteCollector.h"

#define ROUTEVIEW_BASEURL ("http://archive.routeviews.org/") // base url
#define ROUTEVIEW_UPD_DIR ("UPDATES/") // subfolder for the BGP updates
#define ROUTEVIEW_RIB_DIR ("RIBS/") // subfolder for the BGP ribs	-	NOT USED
#define UPD_NAME ("updates.") // update name



/*			ROUTEVIEWRC	CLASS
*
* 	name:				RouteViewRC
* 	description:	It is a derived class from the 'RouteCollector' one.
*						It allows to generate the BGP updates' urls for the RouteView's route collectors.
* 	variables:		-
* 	methods:			getUrl	-	it defines how to generate the urls for the BGP updates coming from the 
*										RouteView's route collectors.
*/
class RouteViewRC : public RouteCollector
{
	public:	RouteViewRC(string, int, tm*);
				virtual void getUrl(bool = true);
};


#endif

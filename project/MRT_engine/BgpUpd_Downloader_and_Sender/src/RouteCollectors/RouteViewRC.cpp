#include "RouteViewRC.h"



/*			CONSTRUCTOR FUNCTION
*
* 	name:				RouteViewRC
* 	description:	It initializes the 'RouteViewRC' object.
* 	parameters:		rcName	-	It specifies the route collector's name which is 
*										used to generate the url.
*							timer	-	It specifies the time interval of the route collector.
*							tm		-	The actual timestamp.
* 	returned:		-
*/
RouteViewRC::RouteViewRC(string rcName, int timer, tm* ptm) : RouteCollector(rcName, timer, ptm)
{
	this->getUrl();
}



/*			GETURL FUNCTION
*
* 	name:				getUrl
* 	description:	It generates the url for the RouteView's BGP updates.
* 	parameters:		n	-	boolean value which specifies if the normalization must be done or not.
* 	returned:		-
*/
void RouteViewRC::getUrl(bool n)
{
	// vars
	char y[5];
  	char m[3], d[3], h[3], min[3];
	int min_norm=0;

	// normalize the timestamp
  	normalize(y, m, d, h, min, min_norm, n);

	// generate the url
	this->lastUrl = string(ROUTEVIEW_BASEURL + rcName + "bgpdata/" + y + "." + m + "/" + ROUTEVIEW_UPD_DIR + UPD_NAME + y + m + d + "." + h + min + ".bz2");
}

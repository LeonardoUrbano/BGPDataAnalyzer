#include "RisRC.h"



/*			CONSTRUCTOR FUNCTION
*
* 	name:				RisRC
* 	description:	It initializes the 'RisRC' object.
* 	parameters:		rcName	-	It specifies the route collector's name which is 
*										used to generate the url.
*							timer	-	It specifies the time interval of the route collector.
*							tm		-	The actual timestamp.
* 	returned:		-
*/
RisRC::RisRC(string rcName, int timer, tm* ptm) : RouteCollector(rcName, timer, ptm)
{
	this->getUrl();
}



/*			GETURL FUNCTION
*
* 	name:				getUrl
* 	description:	It generates the url for the Ris' BGP updates.
* 	parameters:		n	-	boolean value which specifies if the normalization must be done or not.
* 	returned:		-
*/
void RisRC::getUrl(bool n)
{
	// vars
	char y[5];
  	char m[3], d[3], h[3], min[3];
	int min_norm=0;
	
	// normalize the timestamp
  	normalize(y, m, d, h, min, min_norm, n);

	// generate the url
	this->lastUrl = string(RIS_BASEURL + rcName + y + "." + m + "/" + UPD_NAME + y + m + d + "." + h + min + ".gz");
}

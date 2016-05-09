#include "RouteCollector.h"

/*			CONSTRUCTOR FUNCTION
*
* 	name:				RouteCollector
* 	description:	It initializes the 'RouteCollector' object.
* 	parameters:		rcName	-	It specifies the route collector's name which is 
*										used to generate the url.
*							timer	-	It specifies the time interval of the route collector.
*							tm		-	The actual timestamp.
* 	returned:		-
*/
RouteCollector::RouteCollector(string rcName, int timer, tm* ptm)
{
	this->rcName = rcName;
	this->timer = timer;
	this->valid = false;
 	this->lastTm.tm_year=ptm->tm_year;
	this->lastTm.tm_mon=ptm->tm_mon;
	this->lastTm.tm_mday=ptm->tm_mday;
	this->lastTm.tm_hour=ptm->tm_hour;
	this->lastTm.tm_min=ptm->tm_min;
}

void RouteCollector::normalize(char* y, char* m, char* d, char* h, char* min, int& min_norm, bool n)
{
	sprintf(y, "%d", (this->lastTm.tm_year+1900));
  	sprintf(m, "%d", (this->lastTm.tm_mon+1));
  	sprintf(d, "%d", this->lastTm.tm_mday);
  	sprintf(h, "%d", this->lastTm.tm_hour);

	min_norm=this->lastTm.tm_min;
	if(n){
		while((min_norm%this->timer)!=0)
			min_norm--;
		this->lastTm.tm_min=min_norm;
	}
	sprintf(min, "%d", min_norm);
	
	if(min_norm<=9){
		min[1]=min[0];
		min[0]='0';
	}
	if(this->lastTm.tm_hour<=9){
		h[1]=h[0];
		h[0]='0';
		
	}
	if(this->lastTm.tm_mday<=9){
		d[1]=d[0];
		d[0]='0';
	}
	if(this->lastTm.tm_mon<=9){
		m[1]=m[0];
		m[0]='0';
	}
	min[2]='\0';
	d[2]='\0';
	m[2]='\0';
	h[2]='\0';
	y[4]='\0';
}

void RouteCollector::precUrl()
{
	int min=this->timer%60;
	int hour=(int) (this->timer/60);
	if((this->lastTm.tm_hour-hour)<0){
		this->lastTm.tm_hour=24-hour;
		if((this->lastTm.tm_mday - 1) == 0){

				if((this->lastTm.tm_mon-1)<0){// i mesi vanno da 0 a 11
					this->lastTm.tm_mon=11;
					this->lastTm.tm_mday=31;
					this->lastTm.tm_year=this->lastTm.tm_year-1;

				}
				else{

					this->lastTm.tm_mon=this->lastTm.tm_mon-1;
					if(this->lastTm.tm_mon==3 || this->lastTm.tm_mon==5 || 
					   this->lastTm.tm_mon==8 || this->lastTm.tm_mon==10)	//mesi con 30 giorni 
						this->lastTm.tm_mday=30;				
					else{
						if(this->lastTm.tm_mon==1)// febbraio
							this->lastTm.tm_mday=28;
						else
							this->lastTm.tm_mday=31;
					}
				}
		}		
		else
			this->lastTm.tm_mday=this->lastTm.tm_mday-1;
	}else
		this->lastTm.tm_hour=this->lastTm.tm_hour-hour;
	if((this->lastTm.tm_min - min) < 0){

		this->lastTm.tm_min=60 - this->timer;
		if((this->lastTm.tm_hour - 1) < 0){
		
			this->lastTm.tm_hour=23;
			if((this->lastTm.tm_mday - 1) == 0){

				if((this->lastTm.tm_mon-1)<0){// i mesi vanno da 0 a 11
					this->lastTm.tm_mon=11;
					this->lastTm.tm_mday=31;
					this->lastTm.tm_year=this->lastTm.tm_year-1;

				}
				else{

					this->lastTm.tm_mon=this->lastTm.tm_mon-1;
					if(this->lastTm.tm_mon==3 || this->lastTm.tm_mon==5 || 
					   this->lastTm.tm_mon==8 || this->lastTm.tm_mon==10)	//mesi con 30 giorni 
						this->lastTm.tm_mday=30;				
					else{
						if(this->lastTm.tm_mon==1)// febbraio
							this->lastTm.tm_mday=28;
						else
							this->lastTm.tm_mday=31;
					}
				}
			}		
			else
				this->lastTm.tm_mday=this->lastTm.tm_mday-1;
		}
		else
			this->lastTm.tm_hour=this->lastTm.tm_hour-1;

	}else
		this->lastTm.tm_min=this->lastTm.tm_min- min;
	this->getUrl();
}

void RouteCollector::nextUrl(bool n)
{	
	int min;
	int hour;
	if(n){
		min=this->timer%60;
		hour=(int) (this->timer/60);
	}
	else{
		min=1;
		hour=0;	
	}
	if((this->lastTm.tm_hour+hour)>=24){
		this->lastTm.tm_hour=hour-(24-this->lastTm.tm_hour);
		if((this->lastTm.tm_mday==28 && this->lastTm.tm_mon==1) || ((this->lastTm.tm_mon==3 || this->lastTm.tm_mon==5 || this->lastTm.tm_mon==8 || this->lastTm.tm_mon==10) && this->lastTm.tm_mday==30) 				||(this->lastTm.tm_mday==31 && this->lastTm.tm_mon!=11)){
			this->lastTm.tm_mday=1;
			this->lastTm.tm_mon++;
		}
		else{
			if(this->lastTm.tm_mday==31 && this->lastTm.tm_mon==11){
				this->lastTm.tm_year=this->lastTm.tm_year+1;
				this->lastTm.tm_mon=0;
				this->lastTm.tm_mday=1;
			}
			else
				this->lastTm.tm_mday=this->lastTm.tm_mday+1;
		}
		
	}else
		this->lastTm.tm_hour=this->lastTm.tm_hour+hour;
	
	if((this->lastTm.tm_min + min) >= 60){

		this->lastTm.tm_min= min-(60-this->lastTm.tm_min);
		if((this->lastTm.tm_hour + 1) == 24){
		
			this->lastTm.tm_hour=0;
			if((this->lastTm.tm_mday==28 && this->lastTm.tm_mon==1) || ((this->lastTm.tm_mon==3 || this->lastTm.tm_mon==5 || this->lastTm.tm_mon==8 || this->lastTm.tm_mon==10) && this->lastTm.tm_mday==30) ||	(this->lastTm.tm_mday==31 && this->lastTm.tm_mon!=11)){
					
					this->lastTm.tm_mday=1;
					this->lastTm.tm_mon++;
			}
			else{
				if(this->lastTm.tm_mday==31 && this->lastTm.tm_mon==11){
					this->lastTm.tm_year=this->lastTm.tm_year+1;
					this->lastTm.tm_mon=0;
					this->lastTm.tm_mday=1;
				}
				else
					this->lastTm.tm_mday=this->lastTm.tm_mday+1;
			}
		}
		else
			this->lastTm.tm_hour=this->lastTm.tm_hour+1;

	}else
		this->lastTm.tm_min=this->lastTm.tm_min+min;
	this->getUrl(n);
}

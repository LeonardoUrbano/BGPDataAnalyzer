// Global array containing all the charts.
var myLines = new Array();


/*			MYPLOT	OBJECT
*
* 	name:				MyPlot
* 	description:	It defines the 'MyPlot' object.
* 	parameters:				xAxis	-	max value on the x-axis.
*						container	-	<div> where the graph must be contained.
*								index	-	global index of the plot.
*								inOut	-	mode filter for the plot.
* 	returned:		-
*/
function MyPlot(xAxis, container, index, inOut, timeInterval)
{
	// data arrays	
	this.announcement;
	this.withdrawn;
	this.packets;
	// counters
	this.annC = 0;
	this.withC = 0;
	this.packC = 0;
	// plot vars
	this.data;
	this.labels = [];
	this.container = container;
	// other vars
	this.timestamp = -1;
	this.xAxis = xAxis;
	this.index = index;
	this.inOut = inOut;
	this.timeInterval = timeInterval;

	// methods
	this.init = init;
	this.onMessageReceived = myPlotOnMessageReceived;
	this.plotter = plotter;
	this.resetMyPlot = resetMyPlot;
	this.setMode = myPlotSetMode;
	this.setTimeInterval = setTimeInterval;
}
				


/*			INIT	FUNCTION
*
* 	name:				init
* 	description:	It must be called whenever a new 'MyPlot' object is created.
*						It sets the properties of the chart.
* 	parameters:		-
* 	returned:		-
*/
function init()
{
	// init variables	
	var index = this.index;
	var xAxis = this.xAxis;
	this.announcement = new Array(this.xAxis).fill(0);
	this.withdrawn = new Array(this.xAxis).fill(0);
	this.packets = new Array(this.xAxis).fill(0);

	// make a copy for variables' scope issues
	var announcement = this.announcement;
	var withdrawn = this.withdrawn;
	var packets = this.packets;
	var container = this.container;

	// set properties of the chart
	$(function(){						
						var data = [{
											name : 'Withdrawn',
											value: withdrawn,
											color: '#0d8ecf',
											line_width: 2
										},
										{
											name : 'Packets',
											value: packets,
											color: '#ef7707',
											line_width: 2
										},
										{
											name : 'Announcement',
											value: announcement,
											color: '#542c47',
											line_width: 2
										}];							
							// create labels
							var labels = [];
							for(i=0; i<xAxis; i++)
								if(i % 10 == 0)
									labels.push(i);
							labels.push(i);
								
							// create chart
							myLines[index] = new iChart.LineBasic2D({
																		render : container,
																		align:'center',
																		data: data,
																		width : 800,
																		height : 400,
																		background_color: null,
																		tip:	{
																					enable:true,
																					shadow:true
																				},
																		legend :	{
																						enable : true,
																						row:1,
																						column : 'max',
																						valign:'top',
																						sign:'bar',
																						background_color:null,
																						offsetx:-80,
																						border : true
																					},
																		crosshair:	{
																							enable:true,
																							line_color:'#62bce9'
																						},
																		sub_option :	{
																								label:false,
																								point_hollow : false
																							},
																		coordinate:	{
																							width:550,
																							height:300,
																							background_color:'#fff3e6',
																		axis:{
																					color:'#9f9f9f',
																					width:[2,2,2,2]
																				},
																		grids:{
																					vertical:{
																									way:'share_alike',
																							 		value:10
																								}
																				},
																					scale:[	{
																									 position:'left',	
																									 start_scale:0,
																									 end_scale:10000,
																									 scale_space:10000,
																								 	 scale_size:2,
																									 scale_color:'#9f9f9f'
																								},
																								{
																									 position:'bottom',	
																									 labels:labels
																							}]
																			}
																	});
								});
}



/*			PLOTTER	FUNCTION
*
* 	name:				plotter
* 	description:	It loads the new data and it plots the new chart.
* 	parameters:		-
* 	returned:		-
*/
function plotter()
{
	// load data
	var data = [
	{
		name : 'Withdrawn',
		value: this.withdrawn,
		color: '#0d8ecf',
		line_width: 2
	},
	{
		name : 'Packets',
		value: this.packets,
		color: '#ef7707',
		line_width: 2
	},
	{
		name : 'Announcement',
		value: this.announcement,
		color: '#542c47',
		line_width: 2
	}];

	myLines[this.index].load(data);
	// draw plot
	myLines[this.index].draw();
}



/*			RESETMYPLOT	FUNCTION
*
* 	name:				resetMyPlot
* 	description:	It resets all the variables and draws a chart with all zeros.
* 	parameters:		-
* 	returned:		-
*/
function resetMyPlot()
{
	// reset counters
	this.annC = 0;
	this.withC = 0;
	this.packC = 0;
	// reset arrays
	this.announcement.fill(0);
	this.withdrawn.fill(0);
	this.packets.fill(0);
	// draw chart
	this.plotter();
}



/*			MYPLOTONMESSAGERECEIVED	FUNCTION
*
* 	name:				myPlotOnMessageReceived
* 	description:	It is called whenever a new message arrives.
*						Basically, it counts the # of announced subnets, the # of withdraw subnets,
*						the # of packets rerceived during the time interval and it draws the plot.
* 	parameters:				annC	-	# of announcement in the current message.
*								withC	-	# of withdraws in the current message.
*						timestamp	-	timestamp of the current message in seconds.
*						condition	-	boolean values specifies if the current mesage satisfies the condition of the filter.
* 	returned:		-
*/
function myPlotOnMessageReceived(annC, withC, timestamp, condition)
{
	// initialize the timestamp, if it is necessary	
	if(this.timestamp == -1)
		this.timestamp = parseInt(timestamp);
	
	// the message has the same timestamp and it satisfies the filter -> increment counters
	if((parseInt(timestamp) - this.timestamp) < this.timeInterval && condition)
	{
		this.annC += annC;
		this.withC += withC;
		this.packC++;
	}
	// the message has  a timestamp greater then the current by one -> draw plot, update counters
	else if((parseInt(timestamp) - this.timestamp) == this.timeInterval)
	{
		// remove the first element
		this.announcement.shift();
		this.withdrawn.shift();
		this.packets.shift();
		// push the last items
		this.announcement.push(this.annC);
		this.withdrawn.push(this.withC);
		this.packets.push(this.packC);
		
		// the condition is satisfied -> the message must be count
		if(condition)
		{
			this.annC = annC;
			this.withC = withC;
			this.packC = 1;
		}
		// the condition isn't satisfied -> the message must be dropped
		else
		{
			
			this.annC = 0;
			this.withC = 0;
			this.packC = 0;
		}
		// increment timestamp
		this.timestamp += this.timeInterval;
		// draw the graph
		this.plotter();
	}
	// the message has  a timestamp greater then the current one -> draw as many zeros are needed, update counters
	else if((parseInt(timestamp) - this.timestamp) > this.timeInterval)
	{
		this.announcement.shift();
		this.withdrawn.shift();
		this.packets.shift();
		// push the last items
		this.announcement.push(this.annC);
		this.withdrawn.push(this.withC);
		this.packets.push(this.packC);

		for(var i = this.timestamp + this.timeInterval; i < parseInt(timestamp); i+=this.timeInterval) 
		{
			this.timestamp++;
			this.announcement.shift();
			this.withdrawn.shift();
			this.packets.shift();
			// push the last items
			this.announcement.push(0);
			this.withdrawn.push(0);
			this.packets.push(0);
		}
		if(condition)
		{
			this.annC = annC;
			this.withC = withC;
			this.packC = 1;
		}
		else
		{
			
			this.annC = 0;
			this.withC = 0;
			this.packC = 0;
		}
		this.plotter();	
	}

}



/*			MYPLOTSETMODE	FUNCTION
*
* 	name:				myPlotSetMode
* 	description:	It updates the 'inOut' variable of the 'MyPlot' object with the new one.
* 	parameters:		mode	-	new mode.
* 	returned:		-
*/
function myPlotSetMode(mode)
{
	this.inOut = mode;
}



/*			SETTIMEINTERVAL	FUNCTION
*
* 	name:				setTimeInterval
* 	description:	It updates the 'timeInterval' variable of the 'MyPlot' object with the new one.
* 	parameters:		timeInterval	-	new time interval.
* 	returned:		-
*/
function setTimeInterval(timeInterval)
{
	this.timeInterval = timeInterval;
}

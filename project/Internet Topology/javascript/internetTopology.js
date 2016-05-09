// Global variables containing the Graphs and the Charts
var myGraphs = new Array();
var myPlots = new Array();



/*			FILTER	FUNCTION
*
* 	name:				filter
* 	description:	It filters out the entries which not satisfy the condition
* 						on the 'country' or/and on the 'mode' selected.
* 	parameters:		graphChart	-	It specifies the object over which the filter is applied.
*											It may be a 'myGraph' or a 'myPlot' object.
*									i	-	It specifies the index of the object.
*								msg	-	It contains the BGP entry to filter out.
* 	returned:		It returns a boolean value:
*								true	-	the BGP entry satisfies the conditions
*								false	-	otherwise.
*/
function filter(graphChart, i, msg)
{
	//	filter data
		switch(graphChart[i].inOut)
		{
			// INOUT mode	-	'country' is contained in 'IP_from_and_geo' and/or in the announced/withdrawn subnets
			case 0:
				if(msg[0].search(myGraphs[i].country) == -1 && myGraphs[i].country!="all countries" && msg[2].search(myGraphs[i].country)==-1)
					return false;
				break;
			// OUT mode	-	'country' is contained in 'IP_from_and_geo' only
			case 1:
				if(msg[0].search(myGraphs[i].country) == -1 && myGraphs[i].country!="all countries")
					return false;
				break;
			// IN mode	-	'country' is contained in the announced/withdrawn subnets only
			case 2:
				if(msg[2].search(myGraphs[i].country)==-1 && myGraphs[i].country!="all countries")
					return false;
				break;
			// NONE mode	-	all entries are dropped
			case 3:
				return false;
		}
		return true;
}



/*			ONLOAD	FUNCTION
*
* 	name:				-
* 	description:	It performs the following operation:
*						1) fills the <select> dinamically with all the conutries
*						2) creates and initializes two graphs and two charts
*						3) creates and defines a new 'WebSocket' object.
* 	parameters:		-
* 	returned:		-
*/
window.onload = function()
{
	// fill the select
	fillSelect();
	
	// create and initialize graphs and charts
	for(var i=0; i<2; i++)
	{	
		myGraphs[i] = new MyGraph("graph" + (i + 1), "all countries", 0);
		myGraphs[i].setGraphics();
		myPlots[i] = new MyPlot(60, "chart" + (i + 1), i, 0, 1);
		myPlots[i].init();
	}
	myGraphs[1].setMode(3);
	myPlots[1].setMode(3);
	myGraphs[0].setCountry("IT");
	document.getElementsByClassName("select1")[0].value = 'IT';
	// draw chart
	myPlots[0].plotter();

	// create a websocket
	var ws = new WebSocket('ws://127.0.0.1:8082');
	
	// define operations for the websocket's events
	ws.onmessage = function(event)
						{
							// vars
							var msg = event.data.split(";");
							var c;
							// split information fields        
							var IP_from_and_geo=msg[0];
							var WDR_route = msg[1].split(" ");
							var ANN_route_and_geo = msg[2].split(" ");
							var AS_path = msg[3].split(" ");
							
							for(var i=0; i<myGraphs.length; i++)
							{
								// update the UTCtime field
								if(myGraphs[i].graph.getNodesCount() == 0 && myGraphs[i].inOut != 3)
								{
									var d = new Date(parseInt(msg[4]) * 1000);
									document.getElementsByClassName("UTCtime")[i].firstChild.nodeValue = d.toUTCString();
								}
								// update the current time
								var d = new Date(parseInt(msg[4]) * 1000);
								document.getElementsByClassName("divCurrentTime")[i].firstChild.nodeValue = d.toUTCString();
								// update the graphs
								c = filter(myGraphs, i, msg);
								if(c)						
									myGraphs[i].onMessageReceived(msg, IP_from_and_geo, WDR_route, ANN_route_and_geo, AS_path);
								// update the plot								
								c = filter(myPlots, i, msg);
								myPlots[i].onMessageReceived(ANN_route_and_geo.length, WDR_route.length, msg[4], c);
							}
							ws.send("");
   	 				} 

	ws.onopen = function()	{ ws.send(""); }
 
	ws.onclose = function() { } 

	ws.onerror = function() { }
}



/*			CHANGECOUNTRIES	FUNCTION
*
* 	name:				changeCountries
* 	description:	It is called whenever the <select> on the coutry changes its value.
*						Basically, it calls the 'setCountry()' on the corresponding graph
*						and clears both the graph and the plot. 
* 	parameters:		links	-	the <select> element which causes the 'onChange' event.
* 	returned:		-
*/
function changeCountries(links)
{
	// get the new country value
	var val = links[links.selectedIndex].value;
	// get the index of the graph and plot
	var l = links.parentNode.parentNode;
	var id = parseInt(l.id.charAt(5))-1;

	// enable or disable <select> elements
	if (val != "all countries")
		l.getElementsByClassName("select2")[0].disabled = '';
	else
	{
		l.getElementsByClassName("select2")[0].disabled = 'disabled';
		l.getElementsByClassName("select2")[0].value = '0';
	}
	// set country and clear the graph
	myGraphs[id].setCountry(val);
	myGraphs[id].clear();
	// clear the chart
	myPlots[id].resetMyPlot();
}



/*			CHANGEMODE	FUNCTION
*
* 	name:				changeMode
* 	description:	It is called whenever the <select> on the mode changes its value.
*						Basically, it calls the 'setMode()' and, on the base of the value of
*						the checkboxes, clears the graph and/or the plot.
* 	parameters:		links	-	the <select> element which causes the 'onChange' event.
* 	returned:		-
*/
function changeMode(links)
{
	// get the new mode value
	var mode = parseInt(links[links.selectedIndex].value);
	// get the index of the graph and plot
	var l = links.parentNode.parentNode;
	var id = parseInt(l.id.charAt(5))-1;
	// get the <input> checkbox elements
	var checkboxes = document.getElementsByClassName("divViewCheckboxes")[id].getElementsByTagName("input");

	// if the graph is visible, then set the mode and clear the graph
	if(checkboxes[0].checked)
	{	
		myGraphs[id].setMode(mode);
		myGraphs[id].clear();
	}
	// if the plot is visible, then clear the plot
	if(checkboxes[1].checked)
		myPlots[id].resetMyPlot();
}



/*			DISABELBUTONS	FUNCTION
*
* 	name:				disAbleButtons
* 	description:	It is called either by the 'addGraph' function either by the 'removeGraph' one.
*						It enbles mutually exclusive the '+' or '-' buttons.
* 	parameters:		-
* 	returned:		-
*/
function disAbleButtons()
{
	var buttons = document.getElementById("buttons").getElementsByTagName("input");
	for(var i=0; i<buttons.length; i++)
		buttons[i].disabled = (buttons[i].disabled == '')? 'disabled' : '';
}



/*			ADDGRAPH	FUNCTION
*
* 	name:				addGraph
* 	description:	It is called whenever the '+' button is pressed.
*						Basically, it splits the screen in two equal parts and it makes
*						visible a 2nd graph and plot.
* 	parameters:		-
* 	returned:		-
*/
function addGraph()
{	
	// set the mode of the 2nd graph and chart
	document.getElementById("graph1").getElementsByClassName("select2")[0].value = "0";	
	myGraphs[1].setMode(0);
	myPlots[1].setMode(0);
	myPlots[1].setTimeInterval(1);
	// set width of the graphs' containers
	var containers = document.getElementsByClassName("graphContainer");
	containers[0].style.width = '50%';
	//	set visibility of the 2nd graph	
	containers[1].style.display = 'block';
	// set visibility of the 2nd plot
	var chart = document.getElementsByClassName("chartContainer");
	chart[1].style.display = 'block';
	var fieldsetTimeRadio = document.getElementsByClassName("fieldsetTimeRadio");
	fieldsetTimeRadio[1].style.display = 'block';
	var fieldsetCurrentTime = document.getElementsByClassName("fieldsetCurrentTime");
	fieldsetCurrentTime[1].style.display = 'block';
	// set position of the 1st plot
	chart[0].style.left = '0%';
	// able/disable buttons
	disAbleButtons();
	// check checkboxes
	var checkboxes = document.getElementsByClassName("divViewCheckboxes")[1].getElementsByTagName("input");
	for(i=0; i<checkboxes.length; i++)
		checkboxes[i].checked = true;
	// check radiobutton
	var radio = fieldsetTimeRadio[1].getElementsByTagName("input");
	radio[0].checked = true;
}



/*			REMOVEGRAPH	FUNCTION
*
* 	name:				removeGraph
* 	description:	It is called whenever the '-' button is pressed.
*						Basically, it hides the 2nd graph and plot and shrinks the
*						the 1st graph and plot accordingly.
* 	parameters:		-
* 	returned:		-
*/
function removeGraph()
{
	// reset the 2nd graph
	myGraphs[1].reset();
	// set width of the graph's containers to the default one
	var containers = document.getElementsByClassName("graphContainer");
	containers[0].style.width = '100%';
	//	set visibility of the 2nd graph	
	containers[1].style.display = 'none';
	// set visibility of the 2nd plot
	var chart = document.getElementsByClassName("chartContainer");
	chart[1].style.display = 'none';
	var fieldsetTimeRadio = document.getElementsByClassName("fieldsetTimeRadio");
	fieldsetTimeRadio[1].style.display = 'none';
	var fieldsetCurrentTime = document.getElementsByClassName("fieldsetCurrentTime");
	fieldsetCurrentTime[1].style.display = 'none';
	// set position of the 1st plot
	chart[0].style.left = '25%';
	// reset the 2nd plot
	myPlots[1].setMode(3);		
	myPlots[1].resetMyPlot();
	// able/disable buttons
	disAbleButtons();
}



/*			SHOWHIDEGRAPH	FUNCTION
*
* 	name:				showHideGraph
* 	description:	It is called whenever the checkbox relative to the graph is pressed.
*						It shows or hide the graph according the value of the corresponding
*						checkbox.
* 	parameters:		index	-	the index of the graph.
*						show	-	boolean value which specifies if the graph must be shown or not.
* 	returned:		-
*/
function showHideGraph(index, show)
{
	if(!show)
	{	
		myGraphs[index].setMode(3);
		myGraphs[index].clear();
	}
	else
	{
		var mode = parseInt(document.getElementsByClassName("select2")[index].value);		
		myGraphs[index].setMode(mode);
	}
}



/*			SHOWHIDECHART	FUNCTION
*
* 	name:				showHideChart
* 	description:	It is called whenever the checkbox relative to the chart is pressed.
*						It shows or hide the graph according the value of the corresponding
*						checkbox.
* 	parameters:		index	-	the index of the chart.
*						show	-	boolean value which specifies if the chart must be shown or not.
* 	returned:		-
*/
function showHideChart(index, show)
{
	var container = document.getElementsByClassName("chartContainer")[index];
	var fieldsetTimeRadio = document.getElementsByClassName("fieldsetTimeRadio")[index];
	var fieldsetCurrentTime = document.getElementsByClassName("fieldsetCurrentTime")[index];

	container.style.display = (show)? 'block' : 'none';
	fieldsetTimeRadio.style.display = (show)? 'block' : 'none';
	fieldsetCurrentTime.style.display = (show)? 'block' : 'none';
}



/*			CHANGETIMERESOLUTION	FUNCTION
*
* 	name:				changeTimeResolution
* 	description:	It is called whenever the radio buttons relative to the chart are pressed.
*						If the new time resolution is different from the previuos one, it clears the
*						chart and update the 'timeInterval' variable of the 'MyPlot' object.
* 	parameters:					index	-	the index of the chart.
*						timeInterval	-	the new time interval, expressed in seconds.
* 	returned:		-
*/
function changeTimeResolution(index, timeInterval)
{
	// check if the new value differs or not from the previous one
	if(myPlots[index].timeInterval == timeInterval)
		return;
	
	// set the new interval
	myPlots[index].setTimeInterval(timeInterval);
	// clear the plot
	myPlots[index].resetMyPlot();
}

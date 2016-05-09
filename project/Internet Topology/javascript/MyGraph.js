/*			MYGRAPH	OBJECT
*
* 	name:				MyGraph
* 	description:	It defines the 'MyGraph' object.
* 	parameters:		containerId	-	<div> where the graph must be contained.
*							country	-	country filter for the graph.
*								inOut	-	mode filter for the graph.
* 	returned:		-
*/
function MyGraph(containerId, country, inOut)
{
	// variables
	this.country = country;
	this.inOut = inOut;
	this.containerId = containerId;
	// graph initialization	
	this.graph = Viva.Graph.graph();
	// methods
	this.setGraphics = setGraphics;
	this.onMessageReceived = MyGraphOnMessageReceived;
	this.setCountry = setCountry;
	this.setMode = setMode;
	this.clear = clear;
	this.reset = reset;
}



/*			SETGRAPHICS	FUNCTION
*
* 	name:				setGraphics
* 	description:	It must be called whenever a new 'MyGraph' object is created.
*						It sets the actions must be performed when a specific event occurs and
*						it calls the renderer of the graph.
* 	parameters:		-
* 	returned:		-
*/
function setGraphics()
{
	var graph = this.graph;
	var containerId = this.containerId;

	// define onMouseOver callback function
	var graphics = Viva.Graph.View.svgGraphics(), 
						highlightRelatedNodes = function(nodeId, isOn)
														{
															// just enumerate all realted nodes and update link color:
															graph.forEachLinkedNode(nodeId, 
															function(node, link)
															{
																var linkUI = graphics.getLinkUI(link.id);
																if (linkUI)
																{
																	// linkUI is a UI object created by graphics below
																	linkUI.attr('stroke', isOn ? 'red' : 'gray');
																}
															});
														};
	// set callback functions	
	graphics.node(function(node)
	{
		// set images for the nodes
		var country = (node.data == null || node.data.split(';')[0] == '')? 'default' : node.data.split(';')[0];					
	
		var ui = Viva.Graph.svg('image')
					.attr('width', 16)
					.attr('height', 16)
					.link('./img/' + country + '.png');
		
		// set onMouseOver/onMouseOut callback functions			
		$(ui).hover(function()
						{		// mouse over
								highlightRelatedNodes(node.id, true);
 						},
						function()
						{		// mouse out
								highlightRelatedNodes(node.id, false);
						});
		
		// set onClick callback function			
		ui.addEventListener('click', function ()
									{
   	                     	var d = document.getElementById(containerId).getElementsByClassName("info")[0];
										var content = (node.data == null)? 
																"AS: " + node.id : 
																"AS: " + node.id + " " + node.data.substring(0,2) + String.fromCharCode(13) + node.data.substring(3,node.data.length);
   	     							d.firstChild.nodeValue = content;
   	                 		});
					
		return ui;
	});

	// call the renderer
	renderer = Viva.Graph.View.renderer(graph, { 
																graphics : graphics,
																container : document.getElementById(this.containerId)
															 });
	renderer.run();
}



/*			MYGRAPHONMESSAGERECEIVED	FUNCTION
*
* 	name:				MyGraphOnMessageReceived
* 	description:	It is called whenever a new message arrives.
*						Basically, it creates new nodes and links - if it is necessary - and
*						updates either the subnets of the current node either the nodes' counter
*						of the graph.
* 	parameters:						msg	-	it contains the whole message to parse.
*						IP_from_and_geo	-	it contains the IP and the geolocalization of the current node.
*								WDR_route	-	it contains the withdraw subnets of the current node.
*						ANN_route_and_geo	-	it contains the announced subnets of the current node.
*									AS_path	-	it contains the list of ASs through which the message is passed.
* 	returned:		-
*/
function MyGraphOnMessageReceived(msg, IP_from_and_geo, WDR_route, ANN_route_and_geo, AS_path)
{
	// vars
		var i;	
		
	// get current node
		var currentNode = this.graph.getNode(AS_path[0]);	
	// add node to graph and its geolocalization
		if((currentNode == null) || (currentNode.data == null))
			this.graph.addNode(AS_path[0], IP_from_and_geo.split('-')[1] + ";");
		currentNode = this.graph.getNode(AS_path[0]);
	// update subnets of the current node
		// announcement		
			for(i=0; msg[2]!="" && i<ANN_route_and_geo.length; i++)
				if(currentNode.data.search(ANN_route_and_geo[i]) == -1)						
					currentNode.data += ANN_route_and_geo[i] + String.fromCharCode(13);
		// withdraw
			for(i=0; msg[1]!="" && i<WDR_route.length; i++)
			{
				var str = currentNode.data.split(WDR_route[i]);
				if(str.length>1 && str[1] != "")
					currentNode.data = str[0] + str[1];
			}
	
	// add links
	for(i=0; i<AS_path.length-1; i++)
		this.graph.addLink(AS_path[i],AS_path[i+1]);

	// update nodes' counter		
	document.getElementById(this.containerId).getElementsByClassName("nodeC")[0].firstChild.nodeValue = this.graph.getNodesCount();
}



/*			SETCOUNTRY	FUNCTION
*
* 	name:				setCountry
* 	description:	It updates the 'country' variable of the 'MyGraph' object with the new one.
* 	parameters:		country	-	new country.
* 	returned:		-
*/
function setCountry(country)
{
	this.country = country;
}



/*			SETMODE	FUNCTION
*
* 	name:				setMode
* 	description:	It updates the 'inOut' variable of the 'MyGraph' object with the new one.
* 	parameters:		mode	-	new mode.
* 	returned:		-
*/
function setMode(mode)
{
	this.inOut = mode;
}



/*			CLEAR	FUNCTION
*
* 	name:				clear
* 	description:	It removes all nodes and links of the graph.
*						It is called whenever the country or the mode change.
* 	parameters:		-
* 	returned:		-
*/
function clear()
{
	// delete all nodes and links	
	this.graph.clear();
	// clear values in the HTML page
	document.getElementById(this.containerId).getElementsByClassName("nodeC")[0].firstChild.nodeValue = this.graph.getNodesCount();
	document.getElementById(this.containerId).getElementsByClassName("info")[0].firstChild.nodeValue = "";
}



/*			RESET	FUNCTION
*
* 	name:				reset
* 	description:	It resets the whole graph, removing all nodes and links.
*						Then, the mode is set automatically to NONE MODE (mode = 3) in order to drop all the entries
*						regardless the value of both 'country' and 'inOut' variables.
*						It is called when the graph must be cleared and not immediately be recreated (i.e. when the
*						2nd graph must be destroyed).
* 	parameters:		-
* 	returned:		-
*/
function reset()
{
	var select = document.getElementById(this.containerId).getElementsByTagName("select");

	// NONE MODE
	this.setMode(3);
	// set to default values
	this.setCountry("all countries");	
	select[0].value = "all countries";
	select[1].value = "0";
	select[1].disabled = "disabled";
	// delete all nodes and links
	this.clear();
}

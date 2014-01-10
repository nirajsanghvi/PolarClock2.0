var initialized = false;

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  var options = JSON.parse(window.localStorage.getItem("options"));
	
  var url = "http://niraj.com/pebble/polar_clock_config.php";
	
  if(options != null) {
    url += "?time=" + (options["0"] ? encodeURIComponent(options["0"]) : "") + 
           "&date=" + (options["1"] ? encodeURIComponent(options["1"]) : "") +
		   "&rowd=" + (options["2"] ? encodeURIComponent(options["2"]) : "") +
		   "&invt=" + (options["3"] ? encodeURIComponent(options["3"]) : "");
  }
  console.log("showing configuration");
  Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
	if(e.response) {
  		var options = JSON.parse(decodeURIComponent(e.response));
 		console.log("Options = " + JSON.stringify(options));
		
		window.localStorage.setItem("options", JSON.stringify(options));
		
		Pebble.sendAppMessage(options,
			function(e) {
            	console.log("Successfully sent options to Pebble");
            },
            function(e) {
            	console.log("Failed to send options to Pebble.\nError: " + e.error.message);
            }
		);
	}
});
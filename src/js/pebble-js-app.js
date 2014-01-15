var initialized = false;

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  var saveOptions = JSON.parse(window.localStorage.getItem("options"));
	
  var url = "http://niraj.com/pebble/polar_clock_config.php?v1.0";
	
  if(options != null) {
    url += "&time=" + (saveOptions["polarclock0"] ? encodeURIComponent(saveOptions["polarclock0"]) : "") + 
           "&date=" + (saveOptions["polarclock1"] ? encodeURIComponent(saveOptions["polarclock1"]) : "") +
		   "&rowd=" + (saveOptions["polarclock2"] ? encodeURIComponent(saveOptions["polarclock2"]) : "") +
		   "&invt=" + (saveOptions["polarclock3"] ? encodeURIComponent(saveOptions["polarclock3"]) : "") +
		   "&four=" + (saveOptions["polarclock4"] ? encodeURIComponent(saveOptions["polarclock4"]) : "") +
		   "&five=" + (saveOptions["polarclock5"] ? encodeURIComponent(saveOptions["polarclock5"]) : "") +
		   "&six="  + (saveOptions["polarclock6"] ? encodeURIComponent(saveOptions["polarclock6"]) : "");
  }
  console.log("Showing configuration: " + url);
  Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
	if(e.response && e.response.length>5) {
			var options = JSON.parse(decodeURIComponent(e.response));
			
			console.log("Options = " + JSON.stringify(options));
			
			var saveOptions = {
				"polarclock0": options[0],
				"polarclock1": options[1],
				"polarclock2": options[2],
				"polarclock3": options[3],
				"polarclock4": options[4],
				"polarclock5": options[5],
				"polarclock6": options[6]
			}
		
			window.localStorage.setItem("options", JSON.stringify(saveOptions));
			
			Pebble.sendAppMessage(options,
				function(e) {
					console.log("Successfully sent options to Pebble");
				},
				function(e) {
					console.log("Failed to send options to Pebble.\nError: " + e.error.message);
				}
			);
	} else {
		
		console.log("Error with JS Config options received.");	
	}
});
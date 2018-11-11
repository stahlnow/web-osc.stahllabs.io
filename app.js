const osc = require('osc');

// Create an osc.js UDP Port listening on port 57121.
let udpPort = new osc.UDPPort({
	localAddress: "192.168.1.11",
	localPort: 57121,
	remoteAddress: "192.168.1.221",
	remotePort: 8888,
	metadata: true
});


// Listen for incoming OSC bundles.
udpPort.on("message", function (oscBundle, timeTag, info) {
	console.log("An OSC bundle just arrived for time tag", timeTag, ":", oscBundle);
	console.log("Remote info is: ", info);

	udpPort.send({
		timeTag: osc.timeTag(5), // Schedules this bundle 5 seconds from now.
		packets: [
			{
				address: "/pong",
				args: [
					{
						type: "s",
						value: "pong!"
					},
				]
			}]
	}, udpPort.remoteAddress, udpPort.remotePort);

});

// Open the socket.
udpPort.open();



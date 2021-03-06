// CMPT 433 - Assignment 3
// a3-server.js

"use strict";

var socketio = require('socket.io');
var io;

var dgram = require('dgram');

exports.listen = function(server) {
	io = socketio.listen(server);

	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

function handleCommand(socket) {
	// Server Timeout Timer
	var TIMEOUT_MS = 1000;
	var errorTimer = setTimeout(function() {
		socket.emit('errorTimeout', 'timeout');
		console.log('Send Timeout!');
	}, TIMEOUT_MS);

	// Pased string of comamnd to relay
	socket.on('a3-command', function(data) {
		console.log('Client Command: ' + data);

		// Info for connecting to the local process via UDP
		var PORT = 12345;
		var HOST = '192.168.7.2';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		    if (err)
		    	throw err;
		    console.log('UDP message sent to ' + HOST +':'+ PORT);
		});

		client.on('listening', function () {
		    var address = client.address();
		    console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});

		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
		    console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);

		    var reply = message.toString('utf8')
		    socket.emit('commandReply', reply);

		    client.close();
		});

		client.on("UDP Client: close", function() {
		    console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
		    console.log("error: ",err);
		});
		
		clearTimeout(errorTimer);
	});
};

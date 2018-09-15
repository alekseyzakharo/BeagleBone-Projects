// CMPT 433 - Assignment 3
// beatbox-ui.js

"use strict";

// Make connection to server when web page is fully loaded.
var socket = io.connect();

$(document).ready(function() {
	$('#modeNone').click(function(){
		sendCommand("mode-none");
	});
	$('#modeRock1').click(function(){
		sendCommand("mode-rock-1");
	});
	$('#modeRock2').click(function(){
		sendCommand("mode-rock-2");
	});
	$('#modeRock3').click(function(){
		sendCommand("mode-rock-3");
	});
	$('#volumeDown').click(function(){
		sendCommand("volume-down");
	});
    $('#volumeUp').click(function(){
		sendCommand("volume-up");
	});
	$('#tempoDown').click(function(){
		sendCommand("tempo-down");
	});
    $('#tempoUp').click(function(){
		sendCommand("tempo-up");
	});
	$('#sndHiHat').click(function(){
		sendCommand("snd-hihat");
	});
	$('#sndSnare').click(function(){
		sendCommand("snd-snare");
	});
	$('#sndBase').click(function(){
		sendCommand("snd-base");
	});

	var PING_RATE_MS = 1000;
	var pingBBG = setInterval(function() {
		sendCommand("ping-pong");
	}, PING_RATE_MS);

	socket.on('errorTimeout', function(result) {
		$('#error-box').show();
	});

    // On Server Response
	socket.on('commandReply', function(result) {
		console.log("Server Response: " + result);

		var responseArr = result.split(" ");
		var command = responseArr[0];

		if (command === 'ping') {
			// Hide Server Error if it is visible.
			if ($('#error-box').is(":visible")) {
				$('#error-box').hide();
			}
			// Process Ping Response

			var volume = responseArr[1];
			var tempo = responseArr[2];
			var beatMode = responseArr[3];
			var uptime = responseArr[4];

			var beatStr = (beatMode == 0) ? 'None' : ('Rock ' + beatMode);

			$('#volume-id').attr('value', volume);
			$('#tempo-id').attr('value', tempo);
			$('#mode-id').text(beatStr);
			$('#status').text('BeatBox uptime: ' + uptime);
		}
	});

});

function sendCommand(message) {
	socket.emit('a3-command', message);
};

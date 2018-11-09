/*
 * This web application receives data from Kafka Consumer(from WSN)
 * and forward to all clients (index.html) using socket.io
 *
 */

var express = require('express');
var app= express();
var path=require('path');
var http = require('http').Server(app);
var io = require('socket.io')(http);


app.use('/public', express.static(path.join(__dirname, 'public')));
app.use(express.static(__dirname + '/node_modules'));

// send index.html to clients
app.get('/', function(req, res){
    res.sendFile(__dirname + '/index.html');
});


http.listen(8080, function(){
    console.log('listening  port 8080');
});

// Kafka Consumer
var kafka = require('kafka-node'),
    Consumer = kafka.Consumer,
    client = new kafka.Client(),
    consumer = new Consumer(
        client,
        [
            { topic: 'testing1', partition: 0 }//, { topic: 't1', partition: 1 }
        ],
        {
            autoCommit: false
        }
    );
consumer.on('message', function (message) {
    console.log(message.value);
    io.emit('alldata', message);
});

io.on('connection', function(socket)
{
    console.log('A client has been connected');

    socket.on('disconnect', function()
    {
        console.log('A client has been disconnected');
    });
});


// --------------------receive parameters from user --------------------
var args = process.argv.slice(2);
var arduino_command = args[0];
var arduino_ip = args[1];
var arduino_port = args[2];
console.log('arduino command: ' + arduino_command);
console.log('arduino ip: ' + arduino_ip);
console.log('arduino port: ' + arduino_port);
// --------------------create udp --------------------
var udp = require('dgram');

// --------------------creating a udp server --------------------

// creating a udp server
var server = udp.createSocket('udp4');

// emits when any error occurs
server.on('error',function(error){
  console.log('Error: ' + error);
  server.close();
});

// emits on new datagram msg
server.on('message',function(msg,info){
  console.log('Data received from client : ' + msg.toString());
  console.log('Received %d bytes from %s:%d\n',msg.length, info.address, info.port);

//sending msg
server.send(arduino_command,arduino_port,arduino_ip,function(error){
  if(error){
    client.close();
  }else{
    console.log('Data sent !!!' + ' to ' + arduino_ip + ' at ' + arduino_port + ' with msg: ' + arduino_command);
  }

});

});

//emits when socket is ready and listening for datagram msgs
server.on('listening',function(){
  var address = server.address();
  var port = address.port;
  var family = address.family;
  var ipaddr = address.address;
  console.log('Server is listening at port' + port);
  console.log('Server ip :' + ipaddr);
  console.log('Server is IP4/IP6 : ' + family);
    //sending msg
  server.send(arduino_command,arduino_port,arduino_ip,function(error){
    if(error){
      client.close();
    }else{
      console.log('Data sent !!!' + ' to ' + arduino_ip + ' at ' + arduino_port + ' with msg: ' + arduino_command);
    }

  });
});

//emits after the socket is closed using socket.close();
server.on('close',function(){
  console.log('Socket is closed !');
});

server.bind(arduino_port);

setTimeout(function(){
server.close();
},8000);
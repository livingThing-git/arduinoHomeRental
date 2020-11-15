var mysql = require('mysql');
var con = mysql.createConnection({
    host: "localhost",
    user: "root",
    password: "NewPassword",
    database: "duetodue"
});
// code to recieve udp and write record to database
var udp = require('dgram');

// --------------------creating a udp server --------------------

// creating a udp server
var server = udp.createSocket('udp4');

// emits when any error occurs
server.on('error', function(error) {
    console.log('Error: ' + error);
    server.close();
});

// emits on new datagram msg
server.on('message', function(msg, info) {
    console.log('Data received from client : ' + msg.toString());
    console.log('Received %d bytes from %s:%d\n', msg.length, info.address, info.port);
    var device_payload = msg.toString().split(',');
    var emi_id = device_payload[0];
    var voltage = device_payload[1];
    var current = device_payload[2];
    var power = device_payload[3];
    var energy = device_payload[4];
    var freq = device_payload[5];
    var pf = device_payload[6];
    var state = device_payload[7];
    var ip_addr = info.address;
    var write_msg_payload_str = get_msg_payload_insert_Str(emi_id, voltage, current, power, energy, freq, pf, state, ip_addr);
    //con.connect(function(err) {
    // if (err) throw err;
    con.query(write_msg_payload_str, function(err, result, fields) {
        if (err) console.log(err.toString());
        console.log(result);
    });
    //});
    //sending msg
    server.send(msg, info.port, 'localhost', function(error) {
        if (error) {
            client.close();
        } else {
            console.log('Data sent !!!');
        }

    });

});

//emits when socket is ready and listening for datagram msgs
server.on('listening', function() {
    var address = server.address();
    var port = address.port;
    var family = address.family;
    var ipaddr = address.address;
    console.log('Server is listening at port' + port);
    console.log('Server ip :' + ipaddr);
    console.log('Server is IP4/IP6 : ' + family);

});

//emits after the socket is closed using socket.close();
server.on('close', function() {
    console.log('Socket is closed !');
});

server.bind(9766);

//setTimeout(function(){
//server.close();
//},80000);

function get_msg_payload_insert_Str(emi_id, voltage, current, power, energy, freq, pf, state, ip_addr) {
    //there are 9 parameters from arduino.
    //emi_id , voltage, current, power, energy, freq, pf, state, ip
    //pf is power factor.
    //state is status of electricity whether on/off    
    var current_time_stamp = get_current_time_stamp()
    console.log('current time: ' + current_time_stamp)
    var insert_str = "INSERT INTO msg_payload VALUES(\'" + emi_id + "\', " +
        voltage + "," +
        current + "," +
        power + "," +
        energy + "," +
        freq + "," +
        pf + "," +
        ((state == 'off') ? false : true) + ",\'" +
        ip_addr + "\', NOW())";
    console.log(insert_str)
    return insert_str;
}

function get_current_time_stamp() {
    var d = new Date();
    return d.getFullYear() + "" + (d.getMonth() + 1) + "" + d.getDate() + " " + d.getHours() + ":" + d.getMinutes() + ":" + d.getSeconds();
}
var v4 = require("aws-signaturev4-generator");
var mqtt = require("mqtt");
var bodyParser = require('body-parser')
var express = require("express");
var app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());
 
var preSignedUrl = v4.generate_signv4_mqtt("a3f3ep261pa8dz-ats.iot.ap-northeast-1.amazonaws.com", "ap-northeast-1", "AKIAZQKM55U7PUIIUHOX", "AU/VAHPphEiFNl3+0Q5Iil0Fa3lyNE9wrCXzMoSJ");
 
port = 443
 
var client = mqtt.connect(preSignedUrl,
    {
        connectTimeout: 5 * 1000,
        port: port,
    })
 
client.on('connect', function () {
    //client.subscribe(topic, function (err) {
    //    if (!err) {
    //        //client.publish(topic, 'Hello mqtt')
    //        console.log("connected...");
    //    }
    //})
    console.log("connected...");
})
 
//client.on('message', function (topic, message) {
//    console.log(message.toString())
//    i = i + 1
//    client.publish(topic, 'Hello mqtt ' + String(i))
//})
 
app.listen(6666, function () {
    console.log("http://localhost:6666")
})
 
app.post('/command', function (req, res) {
    console.log(req.body);
    console.log(req.body.topic);
    var topic = req.body.topic;
    var message = req.body.message;
    client.publish(topic, message);
    res.end();
});
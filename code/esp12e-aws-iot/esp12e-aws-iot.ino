#include <Arduino.h>
#include <Stream.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

//AWS
#include "sha256.h"
#include "Utils.h"


//WEBSockets
#include <Hash.h>
#include <WebSocketsClient.h>

//MQTT PUBSUBCLIENT LIB 
#include <PubSubClient.h>

//AWS MQTT Websocket
#include "Client.h"
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"

//send IR code
#include <IRremoteESP8266.h>
#include <IRsend.h>
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
// Example of data captured by IRrecvDumpV2.ino
//uint16_t rawDataOn[229] = {3000,3050, 2950,4400, 650,1550, 650,450, 600,1600, 600,550, 550,500, 550,1650, 600,1600, 600,500, 550,550, 550,1650, 600,1550, 650,1550, 650,450, 650,1550, 650,450, 650,450, 550,550, 650,450, 550,550, 550,550, 550,550, 550,550, 550,1650, 650,1550, 600,500, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,1650, 600,500, 600,1550, 650,1550, 650,450, 550,550, 550,1650, 600,500, 550,1650, 600,1600, 600,500, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 500,1650, 650,450, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,1650, 550,600, 500,550, 550,550, 550,550, 550,550, 550,600, 500,550, 550,600, 500,550, 550,550, 550,600, 500,550, 550,550, 550,550, 550,550, 600,500, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,1650, 500,600, 500,1650, 550,1650, 550,1650, 550,550, 550,1650, 550,1650, 550,550, 550,1650, 550,1650, 500};  // UNKNOWN C0798B9E
//uint16_t rawDataOff[229] = {3000,3000, 3000,4400, 600,1600, 600,500, 550,1600, 600,550, 550,550, 550,1600, 550,1650, 650,450, 550,550, 550,1650, 550,1650, 650,1550, 550,550, 550,1650, 550,600, 500,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,1650, 600,450, 550,550, 550,550, 550,600, 500,550, 550,600, 500,550, 550,550, 550,550, 550,550, 550,550, 550,1650, 550,1650, 550,550, 550,1650, 550,1650, 550,550, 550,550, 550,1650, 550,1650, 550,550, 550,550, 550,550, 550,550, 550,500, 550,550, 550,600, 500,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,1650, 550,550, 550,600, 500,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,1650, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,500, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,550, 550,600, 500,550, 550,550, 550,1650, 550,550, 550,1650, 550,1650, 550,550, 550,1650, 550,1600, 550,600, 500,1650, 550,1650, 550,1650, 550}; 

uint16_t rawDataOn[59] = {8400,4000, 500,1500, 500,500, 550,500, 450,550, 450,600, 450,1550, 500,1500, 450,550, 450,600, 450,550, 500,500, 500,550, 450,600, 450,550, 450,550, 450,600, 450,550, 450,550, 500,1550, 450,1550, 450,550, 450,550, 550,1500, 450,550, 500,550, 450,1550, 450,550, 450,1550, 450}; 
uint16_t rawDataOff[59] =  {8350,4000, 500,1550, 450,600, 450,550, 450,600, 450,550, 500,1500, 500,1500, 500,550, 500,1500, 500,1550, 450,550, 450,550, 450,600, 450,550, 500,550, 450,550, 500,500, 500,550, 500,550, 450,550, 500,550, 450,1550, 450,600, 450,1550, 450,550, 450,600, 450,550, 500,1500, 450};


extern "C" {
  #include "user_interface.h"
}

//AWS IOT config, change these:
char wifi_ssid[]       = "your ssid";
char wifi_password[]   = "you wifi password";
char aws_endpoint[]    = "xxxxxx.iot.ap-northeast-1.amazonaws.com";
char aws_key[]         = "aws IAM access key";
char aws_secret[]      = "aws IAM secret access key";
char aws_region[]      = "ap-northeast-1";
const char* aws_topic  = "airconditionertopic"; // set topic, publish message to this topic
int port = 443;

//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

ESP8266WiFiMulti WiFiMulti;

AWSWebSocketClient awsWSclient(1000);

PubSubClient client(awsWSclient);

//# of connections
long connection = 0;

//generate random mqtt clientID
char* generateClientID () {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i]=(char)random(1, 256);
  return cID;
}

//count messages arrived
int arrivedcount = 0;

//callback to handle mqtt messages
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

    
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();

  if (!strncmp((char *)payload,"on",length)){
    Serial.println("on command received..");
    irsend.sendRaw(rawDataOn, sizeof(rawDataOn) / sizeof(rawDataOn[0]), 38);  //Automatically calculate the size of the array, Send a raw data capture at 38kHz.
  }

  if (!strncmp((char *)payload,"off",length)){
    Serial.println("off command received..");
    irsend.sendRaw(rawDataOff, sizeof(rawDataOff) / sizeof(rawDataOff[0]), 38);  //Automatically calculate the size of the array, Send a raw data capture at 38kHz.
  }  

  Serial.println();

  payload[length] = '\0';
  String s = String((char *)payload);

  Serial.println(s);


  
  Serial.println();


}

//connects to websocket layer and mqtt layer
bool connect () {



    if (client.connected()) {    
        client.disconnect ();
    }  
    //delay is not necessary... it just help us to get a "trustful" heap space value
    delay (1000);
    Serial.print (millis ());
    Serial.print (" - conn: ");
    Serial.print (++connection);
    Serial.print (" - (");
    Serial.print (ESP.getFreeHeap ());
    Serial.println (")");


    //creating random client id
    char* clientID = generateClientID ();
    
    client.setServer(aws_endpoint, port);
    if (client.connect(clientID)) {
      Serial.println("connected");     
      return true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      return false;
    }
    
}

//subscribe to a mqtt topic
void subscribe () {
    client.setCallback(callback);
    client.subscribe(aws_topic);
   //subscript to a topic
    Serial.println("MQTT subscribed");
}

//send a message to a mqtt topic
void sendmessage () {
    //send a message   
    char buf[100];
    strcpy(buf, "{\"state\":{\"reported\":{\"on\": false}, \"desired\":{\"on\": false}}}");   
    int rc = client.publish(aws_topic, buf); 
}


void setup() {
    wifi_set_sleep_type(NONE_SLEEP_T);
    Serial.begin (115200);
    delay (2000);
    Serial.setDebugOutput(1);

    //fill with ssid and wifi password
    WiFiMulti.addAP(wifi_ssid, wifi_password);
    Serial.println ("connecting to wifi");
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
        Serial.print (".");
    }
    Serial.println ("\nconnected");

    //fill AWS parameters    
    awsWSclient.setAWSRegion(aws_region);
    awsWSclient.setAWSDomain(aws_endpoint);
    awsWSclient.setAWSKeyID(aws_key);
    awsWSclient.setAWSSecretKey(aws_secret);
    awsWSclient.setUseSSL(true);

    if (connect ()){
      subscribe ();
      sendmessage ();
    }


    //send IR code
    irsend.begin();
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);    

}

void loop() {
  //keep the mqtt up and running
  if (awsWSclient.connected ()) {    
      client.loop ();
  } else {
    //handle reconnection
    if (connect ()){
      subscribe ();      
    }
  }

}

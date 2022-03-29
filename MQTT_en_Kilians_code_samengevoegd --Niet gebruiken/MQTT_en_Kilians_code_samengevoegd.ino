#include "PubSubClient.h" // Connect and publish to the MQTT broker

// Code for the ESP32
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)

// WiFi
const char* ssid = "Devin_hotspot";  // Your personal network SSID
const char* wifi_password = "harpoen1"; // Your personal network password

// MQTT
const char* mqtt_server = "raspberrypi.local";  // IP of the MQTT broker
const char* test_topic = "home/test";
const char* mqtt_username = "test"; // MQTT username
const char* mqtt_password = "password"; // MQTT password
const char* clientID = "client_test"; // MQTT client ID

// Init pins
int IRsensorPin = 3;
int tempSensorPin = 1;

int tempSensorValue = 0;
int IRsensorValue = 0;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(9600);
  connect_MQTT();
}

void loop() {
  Serial.setTimeout(2000);

  IRsensorValue = analogRead(IRsensorPin);
 // tempSensorValue = analogRead(tempSensorPin);
  delay(40);
  
  float i = IRsensorValue;
  float t = tempSensorValue;

//  if(IRsensorValue>1000 && tempSensorValue>1000) {
//    Serial.print("IR = " );
//    Serial.print(IRsensorValue);
//    Serial.print(" and Temp = ");
//    Serial.println(tempSensorValue);
//  }

   if(IRsensorValue>1000) {
    Serial.print("IR = " );
    Serial.print(IRsensorValue);
    }

  if(IRsensorValue<1000) {
    Serial.print("sensor = ");
    Serial.println("SHITS ON FIRE YO");
  }
  
 // if(IRsensorValue<1000 && tempSensorValue<1000){
 //   Serial.print("sensor = " );
 //   Serial.println("SHITS ON FIRE YO");
 // }

  // MQTT can only transmit strings
  String IRstring="IR: "+String((float)i)+" & Temp: "+String((float)t)+" C ";

  // PUBLISH to the MQTT Broker (topic = Test, defined at the beginning)
  if (client.publish(test_topic, String(t).c_str())) {
    Serial.println("Temperature sent!"+String(t));
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(100); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(test_topic, String(t).c_str());
  }

  // PUBLISH to the MQTT Broker (topic = test, defined at the beginning)
  if (client.publish(test_topic, String(i).c_str())) {
    Serial.println("IR Value sent!"+String(i));
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("IR value failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(100); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(test_topic, String(i).c_str());
  }      // print new values every 1 Minute
}

#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)
#include <Arduino.h> //Needed for stuff
#include <TM1637Display.h> //Needed for LED display
#include <TM1637.h>


// Init pins
#define IRsensorPin 2
#define tempSensorPin 4

// LED display
#define CLK 8
#define DIO 9
#define TEST_DELAY  1000 // The amount of time (in milliseconds) between tests

// WiFi
const char* ssid = "COERT DOE DE GANG EENS";  // Your personal network SSID
const char* wifi_password = "Opeenzamehoogte1719"; // Your personal network password

// MQTT
const char* mqtt_server = "10.0.1.4";  // IP of the MQTT broker
const char* temp_topic = "livingroom/t1";
const char* flame_topic = "livingroom/f1";
const char* mqtt_username = "ipmedt5"; // MQTT username
const char* mqtt_password = "password"; // MQTT password
const char* clientID = "client_test"; // MQTT client ID

//Create LED Display opbject
TM1637 tm(CLK, DIO);

// Create WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 

// Custom function to connet to the MQTT broker via WiFi
void connect_WiFi(){
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
}

void connect_broker(){
  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
    delay(100);
  }
}

//LED display function
void displayNumber(int num){
    tm.display(3, 12);      //11 displays a C
    tm.display(2, num % 10);  
    tm.display(1, num / 10 % 10);   
    tm.display(0, num / 100 % 10);
}

void setup() {
  Serial.begin(9600);
  connect_WiFi();
  connect_broker();
}

void loop() {
  Serial.setTimeout(2000);
  
  int IRsensorValue = analogRead(IRsensorPin)/8;
  int tempSensorValue = analogRead(tempSensorPin)/8;

  float t = tempSensorValue / 1024.0 * 100; // Magic volt->Celcius mod
  float i = 0;
  if(IRsensorValue < 700){
    i = 1;
  }

  Serial.print(" IR = ");
  Serial.println(IRsensorValue);

  Serial.print(" Temp = ");
  Serial.println(tempSensorValue);
  //Also display Temp on LED

  displayNumber(t);

  // MQTT can only transmit strings
  String dataTemp=String((float)t);
  String dataFlame=String((float)i);

  // PUBLISH to the MQTT Broker (topic = temp, defined at the beginning)
  if (client.publish(temp_topic, dataTemp.c_str())) {
    Serial.println("Data sent!");
    delay(1000);
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("IR value failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(1000); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temp_topic, String(i).c_str());
  }      // print new values every 1 Minute

  // PUBLISH to the MQTT Broker (topic = flame, defined at the beginning)
  if(client.publish(flame_topic, dataFlame.c_str())) {
    Serial.println("Data sent!");
    delay(1000);
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("IR value failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(1000); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(flame_topic, String(i).c_str());
  }      // print new values every 1 Minute
}

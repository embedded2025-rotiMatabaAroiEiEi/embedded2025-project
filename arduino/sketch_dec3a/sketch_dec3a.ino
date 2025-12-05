#include <WiFi.h>
#include <PubSubClient.h>

//#define APPID       "P418028499295"
#define KEY         "jmtQaLkFRatAbjNW1MZGrUzEPj9oe5cS"
#define SECRET      "DM1rKGj4T5mDAWs3nVYQZW5KMSJXC6Ew"
#define DEVICEID    "60c99d11-739b-4e91-9c99-9d41e68641cb"

const char* ssid = "";  // Wifi name
const char* password = "";  // password

const char* mqtt_server = "mqtt.netpie.io";
const int   mqtt_port   = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

char s;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message from ");
  Serial.print(topic);
  Serial.print(" => ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim(); // remove extra whitespace
  Serial.println(msg);

  // Convert ON/OFF to 1/0
  String uartMsg = "0"; // default
  if (msg == "on") {
    uartMsg = "o";
  } else if (msg =="off") {
    uartMsg = "f";
  }

  // Send single character to STM32 via UART1
  Serial1.print(uartMsg);
  Serial1.flush();
  Serial.print("Sent to STM32: ");
  Serial.println(uartMsg);
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to NETPIE MQTT...");
    
    if (client.connect(DEVICEID, KEY, SECRET)) {
      Serial.println("connected!");
      
      // Subscribe to a channel
      client.subscribe("@shadow/laser/get/accepted");
      client.subscribe("@shadow/laser/update/accepted");
      client.subscribe("@msg/laser");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 3 seconds");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 9, 10); //rx = 9, tx = 10
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }

  if (Serial1.available()) {
    s = Serial1.read();
    Serial.print("Got: ");
    Serial.println(s);
    char msg[2] = { s, '\0' };  
    //client.publish("@feed/light", msg);
  }

  client.loop();

  // ----- Publish example -----
  //client.publish("@msg/esp32", "hello");
  //delay(5000);
}

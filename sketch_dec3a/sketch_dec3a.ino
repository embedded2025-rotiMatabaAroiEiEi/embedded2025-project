#include <WiFi.h>
#include <PubSubClient.h>

//#define APPID       "P418028499295"
#define KEY         "feqYG9dVpcQGNUiMhxis3n8f9Pwvk3Xs"
#define SECRET      "g6XqfPVmvi5pe6K4ywMXTJwy5ygWhbhJ"
#define DEVICEID    "f64f0777-455e-40b6-9bc0-c76f71d5e43b"

const char* ssid = "Test";  // Wifi name
const char* password = "00000000";  // password

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
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to NETPIE MQTT...");
    
    if (client.connect(DEVICEID, KEY, SECRET)) {
      Serial.println("connected!");
      
      // Subscribe to a channel
      client.subscribe("@feed/light");
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
    client.publish("@feed/light", msg);
  }

  client.loop();

  // ----- Publish example -----
  //client.publish("@msg/esp32", "hello");
  //delay(5000);
}

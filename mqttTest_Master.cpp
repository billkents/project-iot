#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "***";
const char* password = "***";
const char* mqtt_server = "***";
const int mqtt_port = 8883; // Port SSL/TLS

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Name", "username", "password")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setInsecure(); // Hanya untuk testing, hindari di produksi
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Contoh: Mengirim perintah untuk menyalakan relay pada slave 1
  if (Serial.available()) {
    char command = Serial.read();
    if (command == '1') {
      client.publish("iotbillkent/wemos/relay1", "ON");
    } else if (command == '0') {
      client.publish("iotbillkent/wemos/relay1", "OFF");
    }
  }
}

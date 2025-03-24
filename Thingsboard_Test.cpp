#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi Credentials
const char* ssid = "***";
const char* password = "***";

// ThingsBoard Server & Token
const char* mqtt_server = "ip-address";
const char* token = "***";

// Relay & LED Pin
#define RELAY_PIN 2
#define TOUCH_PIN 0
#define LEDBLUE 3
#define LEDGREEN 1

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // GMT+7 (25200 detik)

// Jadwal Otomatis (Ubah sesuai kebutuhan)
const int relayOnHour = 10;
const int relayOnMinute = 8;
const int relayOnSecond = 0;
const int relayOffHour = 10;
const int relayOffMinute = 9;
const int relayOffSecond = 0;

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(TOUCH_PIN, INPUT);
    pinMode(LEDBLUE, OUTPUT);
    pinMode(LEDGREEN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LEDBLUE, HIGH);
    digitalWrite(LEDGREEN, HIGH);

    // Koneksi ke WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("WiFi Connected");

    // Setup ThingsBoard MQTT
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    reconnect();

    // Setup NTP Client
    timeClient.begin();
}

void loop() {
    if (!client.connected()) reconnect();
    client.loop();
    
    //timeClient.update();
    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();
    int currentSecond = timeClient.getSeconds();

    int touchState = digitalRead(TOUCH_PIN);

    // Kontrol relay berdasarkan waktu NTP
    if (currentHour == relayOnHour && currentMinute == relayOnMinute && currentSecond == relayOnSecond) {
        digitalWrite(RELAY_PIN, HIGH);
    } else if (currentHour == relayOffHour && currentMinute == relayOffMinute && currentSecond == relayOffSecond) {
        digitalWrite(RELAY_PIN, LOW);
    }

    if (touchState == HIGH) {
    // Jika sensor disentuh, relay ON
    digitalWrite(RELAY_PIN, HIGH);
    } //else {
    // Jika tidak disentuh, relay OFF
    //digitalWrite(RELAY_PIN, LOW);
    //}
}

// Callback MQTT untuk menerima RPC dari ThingsBoard
void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("Pesan RPC: " + message);

    if (message.indexOf("true") > 0) {
        digitalWrite(RELAY_PIN, HIGH);
    } else if (message.indexOf("false") > 0) {
        digitalWrite(RELAY_PIN, LOW);
    }
}

// Fungsi untuk koneksi MQTT
void reconnect() {
    while (!client.connected()) {
        Serial.print("Menghubungkan ke ThingsBoard...");
        if (client.connect("ESP8266_Client", token, NULL)) {
            Serial.println("Berhasil!");
            client.subscribe("v1/devices/me/rpc/request/+" );
        } else {
            Serial.print("Gagal, rc=");
            Serial.print(client.state());
            Serial.println(" coba lagi dalam 5 detik...");
            delay(5000);
        }
    }
}

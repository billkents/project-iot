#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi Configuration
const char* ssid = "***";
const char* password = "***";

// Telegram Configuration
#define BOT_TOKEN "***"
#define CHAT_ID "***"
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// NTP Configuration
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // GMT+7 (25200 detik)

// Pin Definitions & Flags
const int relayPin = D5;
bool lampState[7] = {false, false, false, false, false, false, false};
bool waitingForOnTime = false;
bool waitingForOffTime = false;

// Default waktu nyala dan mati
int onHour = 17, onMinute = 0;
int offHour = 5, offMinute = 0;

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

void lampuNyala() {
  digitalWrite(LED_BUILTIN, LOW);
}

void lampuMati() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  secured_client.setInsecure();
  timeClient.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // NTP Inisialisasi
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  // Timer NTP
  if (currentHour == onHour && currentMinute == onMinute) {
    lampState[1] = true;
    lampuNyala();
  }
  if (currentHour == offHour && currentMinute == offMinute) {
    lampState[1] = false;
    lampuMati();
  }

  // Bot Telegram
  int messageCount = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < messageCount; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    if (chat_id == CHAT_ID) {
      if (text == "/start") {
        bot.sendMessage(CHAT_ID, "Welcome to your personal home lighting control bot! \xF0\x9F\x91\x8B \n\nWith this bot, you can manage your home's lighting from anywhere, anytime \xF0\x9F\x92\xA1 \n\nCreate the perfect ambiance for every moment with a touch of your finger \xF0\x9F\x91\x86", "");
      } else if (text == "/lamp1on") {
        lampState[1] = true;
        lampuNyala();
        bot.sendMessage(CHAT_ID, "Lampu samping is ON", "");
      } else if (text == "/lamp1off") {
        lampState[1] = false;
        lampuMati();
        bot.sendMessage(CHAT_ID, "Lampu samping is OFF", "");
      } else if (text == "/lamp2on") {
        lampState[2] = true;
        bot.sendMessage(CHAT_ID, "Lampu belakang is ON", "");
      } else if (text == "/lamp2off") {
        lampState[2] = false;
        bot.sendMessage(CHAT_ID, "Lampu belakang is OFF", "");
      } else if (text == "/lamp3on") {
        lampState[3] = true;
        bot.sendMessage(CHAT_ID, "Lampu garasi is ON", "");
      } else if (text == "/lamp3off") {
        lampState[3] = false;
        bot.sendMessage(CHAT_ID, "Lampu garasi is OFF", "");
      } else if (text == "/lamp4on") {
        lampState[4] = true;
        bot.sendMessage(CHAT_ID, "Lampu jalan is ON", "");
      } else if (text == "/lamp4off") {
        lampState[4] = false;
        bot.sendMessage(CHAT_ID, "Lampu jalan is OFF", "");
      } else if (text == "/lamp5on") {
        lampState[5] = true;
        bot.sendMessage(CHAT_ID, "Lampu teras atas is ON", "");
      } else if (text == "/lamp5off") {
        lampState[5] = false;
        bot.sendMessage(CHAT_ID, "Lampu teras atas is OFF", "");
      } else if (text == "/lamp6on") {
        lampState[6] = true;
        bot.sendMessage(CHAT_ID, "Lampu kebun atas is ON", "");
      } else if (text == "/lamp6off") {
        lampState[6] = false;
        bot.sendMessage(CHAT_ID, "Lampu kebun atas is OFF", "");
      } else if (text == "/nightmode") {
        for (int i = 1; i <= 6; i++) {
          lampState[i] = true;
        }
        lampuNyala();
        bot.sendMessage(CHAT_ID, "All lights are ON", "");
      } else if (text == "/daymode") {
        for (int i = 1; i <= 6; i++) {
          lampState[i] = false;
        }
        lampuMati();
        bot.sendMessage(CHAT_ID, "All lights are OFF", "");
      } else if (text == "/status") {
        String status = "\xF0\x9F\x92\xA1 Lights Status \xF0\x9F\x92\xA1 \n";
        status += "Lampu samping : " + String(lampState[1] ? "ON" : "OFF") + "\n";
        status += "Lampu belakang : " + String(lampState[2] ? "ON" : "OFF") + "\n";
        status += "Lampu garasi : " + String(lampState[3] ? "ON" : "OFF") + "\n";
        status += "Lampu jalan : " + String(lampState[4] ? "ON" : "OFF") + "\n";
        status += "Lampu teras atas : " + String(lampState[5] ? "ON" : "OFF") + "\n";
        status += "Lampu kebun atas : " + String(lampState[6] ? "ON" : "OFF") + "\n";
        bot.sendMessage(CHAT_ID, status, "");
      } else if (text == "/schedule") {
        char timeStr1[6], timeStr2[6];
        sprintf(timeStr1, "%02d:%02d", onHour, onMinute);
        sprintf(timeStr2, "%02d:%02d", offHour, offMinute);
        String schedule = "The lights are scheduled to turn ON at " + String(timeStr1) + " and turn OFF at " + String(timeStr2);
        bot.sendMessage(CHAT_ID, schedule, "");
      } else if (text == "/set_on") {
        bot.sendMessage(CHAT_ID, "When do you want the lights to turn ON? Send the time in HH:MM format (e.g., 09:05)", "");
        waitingForOnTime = true;
        waitingForOffTime = false;
      } else if (waitingForOnTime) {
        int h, m;
        if (sscanf(text.c_str(), "%2d:%2d", &h, &m) == 2) {
          if (h >= 0 && h < 24 && m >= 0 && m < 60) {
            onHour = h;
            onMinute = m;
            char timeStr[6];
            sprintf(timeStr, "%02d:%02d", onHour, onMinute);
            bot.sendMessage(CHAT_ID, "The ON time is set to " + String(timeStr), "");
            waitingForOnTime = false; // Reset setelah menerima input
          } else {
            bot.sendMessage(CHAT_ID, "Invalid time! Please enter a valid time in HH:MM format (00:00 - 23:59).", "");
          }
        } else {
          bot.sendMessage(CHAT_ID, "Invalid format! Please send the time in HH:MM format (e.g., 09:05).", "");
        }
      } else if (text == "/set_off") {
        bot.sendMessage(CHAT_ID, "When do you want the lights to turn OFF? Send the time in HH:MM format (e.g., 22:30)", "");
        waitingForOffTime = true;
        waitingForOnTime = false; // Reset flag lainnya untuk menghindari konflik
      } else if (waitingForOffTime) {
        int h, m;
        if (sscanf(text.c_str(), "%2d:%2d", &h, &m) == 2) {
          if (h >= 0 && h < 24 && m >= 0 && m < 60) {
            offHour = h;
            offMinute = m;
            char timeStr[6];
            sprintf(timeStr, "%02d:%02d", offHour, offMinute);
            bot.sendMessage(CHAT_ID, "The OFF time is set to " + String(timeStr), "");
            waitingForOffTime = false; // Reset setelah menerima input
          } else {
            bot.sendMessage(CHAT_ID, "Invalid time! Please enter a valid time in HH:MM format (00:00 - 23:59).", "");
          }
        } else {
          bot.sendMessage(CHAT_ID, "Invalid format! Please send the time in HH:MM format (e.g., 22:30).", "");
        }
      }
    } else if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Sorry, you do not have access to this IoT device \xF0\x9F\x9A\xAB", "");
    }
  }
  delay(1000);
}

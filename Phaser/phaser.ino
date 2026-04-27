#include <IRsend.h>
#include <LaserTagCore.h>

// ===== CONFIGURATION =====
uint8_t MY_DEVICE_ID;

const uint16_t IR_LED_PIN = D3;
const uint16_t TRIGGER_PIN = D4;

char* ssid = "LaserTag";
char* password = "lasertag123";
char* serverIP = "192.168.4.1";
int port = 5000;

const String default_url = "http://" + String(serverIP) + ":" + String(port) + "/api";

IRsend irsend(IR_LED_PIN);


// ===== SETUP & LOOP =====
void setup() {
  irsend.begin();
  Serial.begin(115200);
  Serial.println("ESP32-C3 gestartet!");

  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  connectWifi(ssid, password);
  MY_DEVICE_ID = sync(serverIP, port);
}

void loop() {
  if (digitalRead(TRIGGER_PIN) == LOW) {
    shoot(MY_DEVICE_ID);
    delay(20);
    while(digitalRead(TRIGGER_PIN) == LOW);  // Warte auf Loslassen
  }
}

// ===== CALL METHODS =====
void shoot(uint8_t playerID){
  uint8_t checksum = calculate_checksum(playerID);
  uint32_t data = (playerID << 4) | checksum;

  irsend.sendNEC(data, 32);
  Serial.println("Shot fired!");
  sendHitData();
}

uint8_t calculate_checksum(uint8_t playerID){
  uint8_t upper_nibble = (playerID >> 4) & 0b1111;
  uint8_t under_nibble = playerID & 0b1111;
  return (upper_nibble + under_nibble) & 0b1111;
}

void sendHitData(){

  String url = default_url + "/hit/send?id=" + String(MY_DEVICE_ID) + "&time=" + String(millis());
  sendHTTP(url);

}

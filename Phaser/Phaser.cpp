#include <Arduino.h>
#include <IRsend.h>
#include <LaserTagCore.h>
#include <cstdint>

// ===== CONFIGURATION =====
uint8_t DEVICE_UUID[16];
uint8_t PLAYER_ID;
const String type = "Phaser";

const uint16_t IR_LED_PIN = D3;
const uint16_t TRIGGER_PIN = D4;

const String serverIP = "192.168.4.1";
const int port = 5000;

LasertagDevice device(DEVICE_UUID, serverIP, port);
IRsend irsend(IR_LED_PIN);

// ===== CALL METHODS =====
uint8_t calculate_checksum(uint8_t playerID){
  uint8_t upper_nibble = (playerID >> 4) & 0b1111;
  uint8_t under_nibble = playerID & 0b1111;
  return (upper_nibble + under_nibble) & 0b1111;
}

void shoot(uint8_t playerID){
  uint8_t checksum = calculate_checksum(playerID);
  uint32_t data = (playerID << 4) | checksum;

  irsend.sendNEC(data, 32);
  delay(100);
  irsend.sendNEC(data, 32);
  delay(100);
  irsend.sendNEC(data, 32);
  Serial.println("Shot fired!");
  device.sendUDP(LasertagEvent::SHOOT, nullptr, 0);
}


// ===== SETUP & LOOP =====
void setup() {
  irsend.begin();
  Serial.begin(115200);
  Serial.println("ESP32-C3 gestartet!");

  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  device.connectWifi();
  device.sync(LasertagDeviceType::PHASER);
}

void loop() {
  if (digitalRead(TRIGGER_PIN) == LOW) {
    shoot(PLAYER_ID);
    delay(20);
    while(digitalRead(TRIGGER_PIN) == LOW);  // Warte auf Loslassen
  }
}

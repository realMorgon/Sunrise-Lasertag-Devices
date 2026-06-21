#include <Arduino.h>
#include <IRrecv.h>
#include <LaserTagCore.h>

const uint16_t PIN = D4;
IRrecv irrecv(PIN);
decode_results results;
uint8_t DEVICE_UUID[16];
const char* type = "Vest";

String serverIP = "192.168.4.1";
int port = 5000;

LasertagDevice device(DEVICE_UUID, serverIP, port);


void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  irrecv.enableIRIn();

  device.connectWifi();
  device.sync(LasertagDeviceType::VEST);
}

void loop() {
  if (irrecv.decode(&results)){
    // Serial.println("DATA RECIEVED");
    // Serial.print("Value (0-7): ");
    // Serial.println((results.value >> 0) & 0b11111111, BIN);
    // Serial.print("Value (8-15): ");
    // Serial.println((results.value >> 8) & 0b11111111, BIN);
    // Serial.print("Value (DEC): ");
    // Serial.println(results.value, DEC);
    // Serial.print("Value (HEX): ");
    // Serial.println(results.value, HEX);
    // Serial.print("Bits: ");
    // Serial.println(results.bits);

    uint8_t transmitted_checksum = results.value & 0b1111;
    uint8_t data = results.value >> 4;

    uint8_t calculated_checksum = (data & 0b1111) + ((data >> 4) & 0b1111);

    // Serial.print("Data (DEC): ");
    // Serial.println(data, DEC);
    // Serial.print("Data (BIN): ");
    // Serial.println(data, BIN);

    if (calculated_checksum == transmitted_checksum){
      Serial.print("Checksum checks out! (");
      Serial.print(calculated_checksum, BIN);
      Serial.println(")");
      // ===== Send UDP =====
      device.sendUDP(LasertagEvent::GOT_HIT, &data, 1);
      delay(500);
    }else {
      Serial.println("Checksum doesn't check out!");
    }

    Serial.println(" ");

    irrecv.resume();
  }

}

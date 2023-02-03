#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>
#include <SdsDustSensor.h>
#include <HardwareSerial.h>


#define ss 5
#define rst 14
#define dio0 2

/* SDS011 Dust Sensor */
HardwareSerial SerialPort(2);
SdsDustSensor sds(SerialPort);
const int WAKEUP_WORKING_TIME = 30000; 
const int MEASUREMENT_INTERVAL = 60000;
int counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Sender [SDS011]");
  delay(500);

  // setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(866E6))
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF2);
  Serial.println("LoRa Initializing OK!");

  // setup SDS011
  sds.begin();
  Serial.print("SDS011 Initializing OK! ");
  Serial.println(sds.queryFirmwareVersion().toString());
  Serial.println(sds.setQueryReportingMode().toString());
}

void loop() {
  // Wake up SDS011
  sds.wakeup();
  Serial.println("SDS011 sensor is awake");
  delay(WAKEUP_WORKING_TIME);
  PmResult pm = sds.queryPm();

  if (pm.isOk()) {
    Serial.print("PM2.5[μg/m3] = ");
    Serial.print(pm.pm25);
    Serial.print(", PM10[μg/m3] = ");
    Serial.println(pm.pm10);

    // LoRA packet
    Serial.print("Sending packet: ");
    Serial.println(counter);
    LoRa.beginPacket();
    LoRa.print("\nPM2.5[μg/m3] = ");
    LoRa.print(pm.pm25);
    LoRa.print("\nPM10[μg/m3] = ");
    LoRa.println(pm.pm10);
    LoRa.print(counter);
    LoRa.endPacket();
    counter++;
    Serial.print("Packet sent");
  }else {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }

  // Put SDS011 back to sleep
  WorkingStateResult state = sds.sleep();
  if (state.isWorking()) {
    Serial.println("Problem with sleeping the SDS011 sensor.");
  } else {
    Serial.println("SDS011 sensor is sleeping");
    delay(MEASUREMENT_INTERVAL);
  }
}
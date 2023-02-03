#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Reciver [SDS011]");
  delay(500);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println("SSD1306 allocation failed");
  }
  // display.display();
  // delay(2000);
  // display.clearDisplay();

  // setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(866E6))
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF2);
  Serial.println("LoRa Initializing OK!");
}

void loop()
{
  // try to parse packet
  String LoRaData;
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available())
    {
      display.clearDisplay();
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
      display.setTextSize(1);
      display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
      display.setCursor(0, 10);
      // Display text
      display.print(LoRaData);
      display.print("\nwith RSSI: ");
      display.print(LoRa.packetRssi());
      display.display();
    }
  }
}
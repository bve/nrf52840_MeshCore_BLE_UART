#include <Arduino.h>
#include <HardwareSerial.h>
#include <Adafruit_TinyUSB.h>
#include <bluefruit.h>
#include <BLEBridge.h>
#include <led.h>

SerialBLEInterface bleInterface;
BLEBridge *bleBridge = nullptr;

void setup()
{
  ledSetup();
  Serial1.begin(115200);
  Serial1.println("BLE Bridge Starting...");
  bleBridge = new BLEBridge(&Serial1, bleInterface);
}

void loop()
{
  bleBridge->loop();
  delay(100);
}

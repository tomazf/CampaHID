/*
  Jantar RFID reader - VCOM to HID

  version: v1 - 14.6.2021 by Ferbi

  For the Arduino Leonardo, Micro or Due

  The circuit:
   - jumper selects MODE (RAW/parsed) on modePIN

*/

#include "Keyboard.h"

#define DEBUG false

uint8_t modePIN = 2;            // HW mode pin
const uint8_t DATAlength = 18;  // received data length
const uint8_t IDlength = 6;     // useful ID length
bool pinVal;                    // mode selector
uint8_t id_start = 6;           // subsection for ID in original buffer
uint8_t offset = 0;             // serial bytes offset

unsigned long previousMillis = 0;    // will store last time loop was updated
const long interval = 100;           // interval for serial readout

void setup() {

  // open serial ports
  if (DEBUG) Serial.begin(9600);
  Serial1.begin(9600);

  // set mode pin as input
  pinMode(modePIN, INPUT_PULLUP);
  delay(500);

  // read pin only on startup
  pinVal = digitalRead(modePIN);

  // initialize control over the keyboard - start HID
  Keyboard.begin();
}

void loop() {

  // delay for buffer to settle
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    // check for incoming serial data
    int numBytes  = Serial1.available();
    if (numBytes >= DATAlength)
    {
      // custom read
      char buffer[numBytes];
      for (int i = 0; i <= numBytes; i++)
      {
        buffer[i] = Serial1.read();
      }
      String incomingString((char*)buffer);
      offset = incomingString.indexOf("40FF");      // it seems that offset is always 0, beacuse we empty serial buffer, just in case

      if (offset < 255)
      {
        if (pinVal != LOW)
        {
          for (int i = offset; i < DATAlength + offset ; i++)
          {
            Keyboard.write(incomingString[i]);      // RAW output
          }
          Keyboard.println("");
        }
        else
        {
          String subBuffer = incomingString.substring( offset + id_start, offset + id_start + IDlength);
          Keyboard.print(strtol(subBuffer.c_str(), NULL, 16));    // convert substring subBuffer to decimal (from HEX)
          Keyboard.println("");
        }
      }
    }
  }
}

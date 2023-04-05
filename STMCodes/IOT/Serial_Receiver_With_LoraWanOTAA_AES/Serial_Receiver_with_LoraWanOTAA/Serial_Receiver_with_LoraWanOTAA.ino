/* Wire Master example (see Wire_Slave.ino for the master)
 *    
 * The code in Wire_Slave.ino implements a simple slave
 * device that transmits a recurring data stream. Hence
 * first initialize the index into the stream, and then
 * read it in 8 byte chunks, and then print it.
 *
 *    
 * This example code is in the public domain.
 */
//Receiver library
#include "Wire.h"


//LoRaWan OTAA Library
#include "LoRaWAN.h"
#define REGION_US915
//App key and stuff from TTN (Requires change to identify devices)
const char *appEui  = "0909090909090909";
const char *appKey  = "92D75C9F85182775460B649795464A84";
const char *devEui  = "70b3d57ed005c1d3";



void setup()
{
  //Serial setup
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("BEGIN INIT");



  //Receive from another device--------------------------------------
  Wire.begin();
  Wire.beginTransmission(0x7c);
  Wire.write(0x00);
  Wire.endTransmission();

  //LoraWan---------------------------------------------------------
  #if defined(REGION_US915)
    LoRaWAN.begin(US915);
    LoRaWAN.setSubBand(2);
  #endif
  // LoRaWAN.setDutyCycle(false);
  // LoRaWAN.setAntennaGain(2.0);
  LoRaWAN.joinOTAA(appEui, appKey, devEui);
  Serial.println("JOIN( )");
}

void loop()
{
    int size;
    uint8_t data[32] = {0};

    size = Wire.requestFrom(0x7c, 32);

    if (size)
    {
      Wire.readBytes(data, 32);
      String message = "";
      for(int i =0; i < sizeof(data); ++i)
      {

        // if(data[i] >= 48 && data[i] <=57 ||data[i] ==45 || data[i] == 13 || data [i] == 10)
        if(data[i] >= 32 && data[i] <= 175 || data[i] == 13 || data [i] == 10)
        {
          Serial.print((char)data[i]); //Print out readable text and numbers (Replace with RSSI sending code)
          message += (char)data[i];

          // Serial.println(message);
          // Serial.println(message.toInt());

        }
        if (i == sizeof(data))
          memset(data, '\0', 32);
      }

      message = String(message.toInt() ^ 303030);
      sendViaOTAA(message);
    }

    //Delay
    delay(1000);
}

void sendViaOTAA(String message)
{
  char utf8String[message.length() + 1];
  message.toCharArray(utf8String, sizeof(utf8String));

  if (LoRaWAN.joined() && !LoRaWAN.busy())
    {
        Serial.print("TRANSMIT( ");
        Serial.print("TimeOnAir: ");
        Serial.print(LoRaWAN.getTimeOnAir());
        Serial.print(", NextTxTime: ");
        Serial.print(LoRaWAN.getNextTxTime());
        Serial.print(", MaxPayloadSize: ");
        Serial.print(LoRaWAN.getMaxPayloadSize());
        Serial.print(", DR: ");
        Serial.print(LoRaWAN.getDataRate());
        Serial.print(", TxPower: ");
        Serial.print(LoRaWAN.getTxPower(), 1);
        Serial.print("dbm, UpLinkCounter: ");
        Serial.print(LoRaWAN.getUpLinkCounter());
        Serial.print(", DownLinkCounter: ");
        Serial.print(LoRaWAN.getDownLinkCounter());
        Serial.println(" )");

        LoRaWAN.beginPacket();
        LoRaWAN.write((uint8_t*)utf8String, 13);
        LoRaWAN.endPacket();

    }
}

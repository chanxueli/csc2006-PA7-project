/* Wire Slave example (see Wire_Master.ino for the master)
 *    
 * The myReceiveCallback tracking the received data sets
 * tx_index into tx_data[], if only one byte had been
 * transferred. The myRequestCallback puts 32 bytes from
 * tx_data[] starting at tx_index into the transmit buffer.
 * Finally the myTransmitCallback adjusts tx_index with
 * the number of transferred bytes.
 *
 * The code roughly simluates a slave device with a FIFO.
 * Sunce the myRequestCallback cannot know how many bytes
 * need to be send, it fills up the buffer to the max. 
 * Only at the myTransmitCallback the number  of bytes
 * transmitted is known.
 *
 *    
 * This example code is in the public domain.
 */

//Serial Writer includes
#include "Wire.h"
int tx_index = 0;
uint8_t tx_data[] = "0000\r\n";
bool looponce = true;
// int number = 0;


// B-L072Z-LRWAN1 library file (Lora Radio)
#include <SPI.h>
#include "LoRaRadio.h"
//File Defines
#define SERIAL 9600       //Baud Rate
#define RF_FREQ 923300000 //Freq
#define TX_PWR 14         //5 to 23 dBm
#define TIMEOUT 5000      //Set timeout to 5s



//Get request from the other msp with data on how much byte it can accept
void myReceiveCallback(int count)
{
    if (count == 1)
    {
        tx_index = Wire.read();
        
        while (tx_index >= sizeof(tx_data))
        {
            tx_index -= sizeof(tx_data);
        }
    }
}
//Wire writer to write to another device
void myRequestCallback(void)
{
  if(!looponce)
  {
    return;
  }
  // Serial.print("tx_index= ");
  // Serial.println(tx_index);
  for (int i = 0, n = tx_index; i < BUFFER_LENGTH; i++)
  {
    Wire.write(tx_data[n]);

    n++;
    
    if (n >= sizeof(tx_data)) { n = 0; break;}
  }
  delay(1000);
  
}
//Wire transmit counter
void myTransmitCallback(int count)
{
  tx_index += count;
  // Serial.println(count);
  if(tx_index >= sizeof(tx_data))
  {
    looponce = false;
  }

  while (tx_index >= sizeof(tx_data))
  {
      tx_index -= sizeof(tx_data);
  }
}

void setup()
{
  //Serial (Print on console)
  Serial.begin(SERIAL);
  while(!Serial){}
  Serial.println("BEGIN INIT");
  //Write to the other device via cable (Setup)----------------------------
  Wire.begin(0x7c);
  Wire.onReceive(myReceiveCallback);
  Wire.onRequest(myRequestCallback);
  Wire.onTransmit(myTransmitCallback);

  //Init the LoraRadio-----------------------------------------------------
  //Begin LORA Radio frequency
  LoRaRadio.begin(RF_FREQ);
  LoRaRadio.setFrequency(RF_FREQ);
  //you can set transmitter powers from 5 to 23 dBm:
  LoRaRadio.setTxPower(TX_PWR);
  //Lora default radio bandwidth
  LoRaRadio.setBandwidth(LoRaRadio.BW_125);
  //Spreading Factor (7-12) (Higher = less data longer range)
  LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
  //LNA boost to enable lora to detect weaker lora signals
  LoRaRadio.setLnaBoost(true);

  //Setup LoRa Receive & Timeout
  LoRaRadio.receive();
}

//Use this function to send to the other msp
void sendMessageThroughWire(String message)
{
  for (int i = 0; i < sizeof(tx_data); ++i) {
    tx_data[i] = '\0';
  }
  looponce = true;
  tx_index = 0;
  String messageNew = message + "\r\n";
  uint8_t len = messageNew.length();
  // copy the message into the tx_data array
  memcpy(tx_data, messageNew.c_str(), len);
}

void loop()
{
  // Wait for a message
  if (LoRaRadio.parsePacket()) {
    // Print the received message
    String message = "";
    while (LoRaRadio.available()) {
      message += (char)LoRaRadio.read();
    }
    // Serial.println("Message received: " + message);
    int radioRSSI = LoRaRadio.packetRssi();
    sentRSSI(radioRSSI);
    // Serial.println(radioRSSI);
  }
  else {
    // Serial.println("Waiting");
    delay(500);
  }
}

void sentRSSI(int data){
  char str[6];
  sprintf(str, "%d", data);
  sentData(str);

  //Serial write to the other device if available to write
  if(!looponce)
  {
    sendMessageThroughWire(String(data));
    // number++;
  }
}

void sentData(char data[]){
 char headerBreak[] = "%d/"; 
 strcat(headerBreak, data);
 delay (20);
 Serial.println(headerBreak);
 delay (20);
}


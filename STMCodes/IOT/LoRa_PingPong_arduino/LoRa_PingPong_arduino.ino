
/* Simple Ping-Pong for a LoRa Radio/Modem
 *
 * In setup() below please adjust your country specific frequency ranges,
 * as well as the Bandwidth/SpreadingFactor/CodingRate settings.
 *
 * They way this example works is that the device first listens for 5000ms.
 * If it received a "PING" message, it considers itself a SLAVE. If not
 * it considers itself a MASTER. A SLAVE waits for an incoming "PING" message,
 * which it answers with a "PONG" message. A MASTER simply sends periodically
 * every 1000ms) a "PING" message, and collects "PONG" replies while waiting.
 *    
 *    
 * This example code is in the public domain.
 */
 
#include "LoRaRadio.h"


#define STATE_NONE        0
#define STATE_SCANNING    1
#define STATE_TX_MASTER   2
#define STATE_RX_MASTER   3
#define STATE_TX_SLAVE    4
#define STATE_RX_SLAVE    5

int state = STATE_NONE;

void setup() {
  Serial.begin(9600);
    
  while (!Serial) { }
  LoRaRadio.begin(923300000);
  LoRaRadio.setFrequency(923300000);
  LoRaRadio.setTxPower(2);
  LoRaRadio.setBandwidth(LoRaRadio.BW_125);
  LoRaRadio.setSpreadingFactor(LoRaRadio.SF_7);
  LoRaRadio.setCodingRate(LoRaRadio.CR_4_5);
  LoRaRadio.setLnaBoost(true);
}

void loop(void) {
  package();
  delay(500); // send a "PING" every second
  Serial.println("Hello it works");  
}


void package(){
  
  // uint32_t crc = calcCrc(data, sizeof(data));
  LoRaRadio.beginPacket();
  
  LoRaRadio.write('Jesus is our savior');

  LoRaRadio.endPacket();
}



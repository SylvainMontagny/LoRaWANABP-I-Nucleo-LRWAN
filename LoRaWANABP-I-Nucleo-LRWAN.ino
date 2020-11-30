#include "LoRaWANNode.h"
#include <stdio.h>
#include <stdlib.h>

#define PUSHBUTTON  PC13
#define FRAME_DELAY_BY_TIME    false  // Sending method (TIME or PUSH BUTTON)    
#define FRAME_DELAY         5000      // Time between 2 frames
#define DATA_RATE           4
#define ADAPTATIVE_DR       true
#define ENABLE              1
#define DISABLE             0
#define  CONFIRMED          false
#define PORT                1

HardwareSerial SerialLora(D0, D1); // D0(Rx) D1(TX)
HardwareSerial Serial1(PA10, PA9);

// Device address, network & application keys
const char devAddr[] = "26013A7B";
const char nwkSKey[] = "4D02816CFFE64FEA1599C281982FC976";
const char appSKey[] = "614BE9D577FC176D84816FE77849975C";

char frameTx[] = "Hello";
String str;

void setup()
{
  Serial1.begin(115200);
  pinMode(PUSHBUTTON, INPUT);
  infoBeforeActivation();
  Serial1.println(" Checking activation on LoRaWAN Server in progress...");  

  while(!loraNode.joinABP(devAddr, nwkSKey, appSKey)) {
    Serial1.println(" The Device is not activated on the Server!!");
    delay(1000);
  }
   Serial1.println(" Device activated on the Server : SUCCESS!!\r\n");
  infoAfterActivation();
}

void loop()
{
  if( FRAME_DELAY_BY_TIME == 0)   while(digitalRead(PUSHBUTTON)); // Attente Push Button pour envoyer
  else                            delay(FRAME_DELAY);             // Attente FRAME_DELAY pour envoyer
  Serial1.print(" Sending Text : \"");Serial1.print(frameTx);Serial1.print("\"");
  if(CONFIRMED)   Serial1.print(" Uplink CONFIRMED on PORT ");
  else            Serial1.print(" Uplink UNCONFIRMED on PORT ");
  Serial1.println(PORT);
  transmit();
  receive();
}

void receive(void) {
  uint8_t frameRx[64];
  uint8_t len;
  uint8_t port;

  if(loraNode.receiveFrame(frameRx, &len, &port)) {
    uint8_t n = 0;
    Serial1.print(" Frame received: 0x");
    while(len > 0) {
      Serial1.print(frameRx[n], HEX);
      Serial1.print(',');
      len--;
      n++;
    }
    Serial1.print(" on port "); Serial1.println(port);Serial1.print("\r\n");
  } else {
    Serial1.println(" No data received\r\n\r\n");
  }
}

void transmit(void) {
  
  int status = loraNode.sendFrame(frameTx, sizeof(frameTx), CONFIRMED, PORT);
  if(status == LORA_SEND_ERROR) {
    Serial1.println(" Send frame failed!!!");
  } else if(status == LORA_SEND_DELAYED) {
    Serial1.println(" Module is busy : \r\n * It's still trying to send data \r\n OR * \r\n * You are over your allowed duty cycle");
  } else {
    Serial1.println(" Frame sent");
  }
}

void infoBeforeActivation(void){
  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("#######################################");
  Serial1.println("######## Formation LoRaWAN ############");
  Serial1.println("######### ABP activation  ##########\r\n");

  // Enable the USI module and set the radio band.
  while(!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
    Serial1.println("Lora module not ready");
    delay(1000);
  }
  
  loraNode.setDutyCycle(false);
  str = " * Device Address :      0x ";
  loraNode.getDevAddr(&str);
  Serial1.println(str);
  str = " * Network Session Key : 0x ";
  loraNode.getNwkSKey(&str);
  Serial1.println(str);
  str = " * Application Session Key : 0x ";
  loraNode.getAppSKey(&str);
  Serial1.println(str);Serial1.print("\r\n");
  
 loraNode.setAdaptativeDataRate(DISABLE);
 loraNode.setDataRate(DATA_RATE);
 Serial1.print(" * Data Rate : ");Serial1.print(loraNode.getDataRate());Serial1.print("\r\n");
 if(ADAPTATIVE_DR) {
    loraNode.setAdaptativeDataRate(ENABLE);
    Serial1.print(" * Adaptative Data Rate : ON");Serial1.println("\r\n");
  }
  else {
    Serial1.print(" * Adaptative Data Rate : OFF");Serial1.println("\r\n");
  }       
  loraNode.setDutyCycle(DISABLE);
}

void infoAfterActivation(void){
   if(FRAME_DELAY_BY_TIME == 1){
    Serial1.print(" Frame will be sent every");Serial1.print(FRAME_DELAY);Serial1.println("\r\n");
  }
  else {
    Serial1.println(" Press Blue Button to send a Frame\r\n");
  }
  
}

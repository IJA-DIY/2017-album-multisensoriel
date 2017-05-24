/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * 
 * Typical pin layout used:
 * --------------------------------------
 *             MFRC522      Teensy 3.6     
 *             Reader/PCD   
 * Signal      Pin          Pin          
 * ---------------------------------------
 * RST/Reset   RST          9    31         
 * SPI SS      SDA(SS)      10   33 ou 32         
 * SPI MOSI    MOSI         11   28
 * SPI MISO    MISO         12   39
 * SPI SCK     SCK          13   27
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>


#define SS_PIN 3
#define RST_PIN 4
//#define SS_PIN_AUDIO 18


 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 




// Init array that will store new NUID 
byte nuidPICC[4];
byte* a,b;

void setup() { 
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  SPI.setMOSI(11);
  SPI.setSCK(13);



  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  //Serial.println(F("This code scan the MIFARE Classsic NUID."));
  //Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}
 
void loop() {

  
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    //Serial.println(F("msg=A new card has been detected."));
    //Serial.println("playing sound");
    

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    //Serial.print(F("tag="));
    a = rfid.uid.uidByte, b =  rfid.uid.size;
    printHex(a,b);
   // Serial.print(a[0], DEC);
    Serial.println();
    
  }
  //else 
 //Serial.println(F("msg=Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

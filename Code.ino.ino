#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <MFRC522.h>


AudioPlaySdWav           playWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
//AudioOutputAnalog      audioOutput;
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;


#define NB_TAG 12

#define SS_PIN 3
#define RST_PIN 4
#define SS_PIN_AUDIO 18


// Use these with the audio adaptor board
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// Use these for the SD+Wiz820 or other adaptors
//#define SDCARD_CS_PIN    4
//#define SDCARD_MOSI_PIN  11
//#define SDCARD_SCK_PIN   13

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 


int ind_bcl=0;
int tab_nbson={4,1,4,6,1,4,1,1,1,1};
int nb_son;

//// Init array that will store new NUID 
byte nuidPICC[4];
byte* a,b;

//on stock les numeros des tags dans une matrice
byte tagnfc[NB_TAG][4];

//on stock les series d'audio dans une matrice
const char * filelist[NB_TAG][nb_son]={{"Luce.wav", "sean.wav", "cony.wav","Laure.wav"},{"Amanda.wav"},{"Ruben.wav","Liam.wav","Maé.wav","Maebh.wav"},{"Timothy.wav","Niels.wav","Renaud.wav","Liam Evan.wav","Theo.wav","Maéthan.wav"},{"Killian.wav"},{"AmandaE.wav","Idriss.wav","Ida.wav","Andréa.wav"}{"100 Jours DESCRIPTION.wav"},{"100 Jours.wav"},{"jeux bonhomme description.wav"},{"Jeux Bonhomme photo.wav"}};


//booléen qui valide si un tag correspond a une serie d'audio
boolean estvalide= false;


void setup() {
  Serial.begin(9600);
  // déclarer les pins SS en sortie
  pinMode(SS_PIN, OUTPUT);
  pinMode(SS_PIN_AUDIO, OUTPUT);  

  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  SPI.setMOSI(7); // Pourquoi ?
  SPI.setSCK(14);  // Pourquoi ?

  digitalWrite(SS_PIN, LOW); // On décide de commencer par la lecture de tag
  digitalWrite(SS_PIN_AUDIO, HIGH); // On  désactive l'audio
}


void playFile(const char *filename) //sert à jouer les audios
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);
}

/* Helper routine to dump a byte array as hex values to Serial. 
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


void loop() {
  int filenumber = 0;
  if (SS_PIN == LOW) { // on peut lire un tag
    // Partie TAG
    // // Look for new cards
    if ( ! rfid.PICC_IsNewCardPresent())
      return;

    // Verify if the NUID has been readed
    if ( ! rfid.PICC_ReadCardSerial())
      return;
    while(ind_bcl<NB_TAG || estvalide=false )
    {
      if (rfid.uid.uidByte[0] == tagnfc[ind_bcl][0] || 
        rfid.uid.uidByte[1] == tagnfc[ind_bcl][1]] || 
        rfid.uid.uidByte[2] == tagnfc[ind_bcl][2] || 
        rfid.uid.uidByte[3] == tagnfc[ind_bcl][3] ) {
        Serial.println(F("msg=A card has been detected."));
        Serial.println("playing sound");
        nb_son=tab_nbson[ind_bcl];
        estvalide=true;
        
        
      // Store NUID into nuidPICC array
      for (byte i = 0; i < 4; i++) {
        nuidPICC[i] = rfid.uid.uidByte[i];
      }
   
      Serial.print(F("tag="));
      a = rfid.uid.uidByte, b =  rfid.uid.size;
      
      printDec(a,b);
      Serial.print(a[0], DEC);
      Serial.println();
        
      // OK MAINTENANT ALLER SUR L'AUDIO
      digitalWrite(SS_PIN,HIGH);
      digitalWrite(SS_PIN_AUDIO,LOW); 
    }
    else Serial.println(F("msg=Card read previously."));

    }
  }
  else if (SS_PIN_AUDIO == LOW) {
    if (!(SD.begin(SDCARD_CS_PIN))) {
      // stop here, but print a message repetitively
      while (1) {
        Serial.println("Unable to access the SD card");
        delay(500);
      }
    }
    //Playfile

    while (filenumber<nb_son)
    {
      const char *filename = filelist[ind_bcl][filenumber]
    
      playFile(filename);  // filenames are always uppercase 8.3 format
      filenumber = filenumber + 1;
      if (filenumber >= 3)filenumber=0;
      delay(10);
      // read pushbuttons
        button0.update();
        if (button0.fallingEdge()) 
        {
          playSdWav1.stop();
        }
        button2.update();
        if (button2.fallingEdge()) 
        {
          playSdWav1.stop();
          filenumber = filenumber - 2;
          if (filenumber < 0) filenumber = 0;
        }
    }
    // On repasse en mode lecture de TAG
    digitalWrite(SS_PIN,LOW); // On repasse vers le TAG
    digitalWrite(SS_PIN_AUDIO,HIGH); 
  }
}

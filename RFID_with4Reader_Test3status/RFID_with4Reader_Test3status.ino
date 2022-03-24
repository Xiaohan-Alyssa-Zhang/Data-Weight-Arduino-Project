#include <SPI.h>
#include <MFRC522.h>

// PIN Numbers : RESET + SDAs
#define RST_PIN         49
#define SS_1_PIN        53
#define SS_2_PIN        48
#define SS_3_PIN        46
#define SS_4_PIN        47

#define NR_OF_READERS   4

byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN};
String current_Status[]= {"Unknow","Unknow","Unknow","Unknow"};
// Create an MFRC522 instance :
MFRC522 mfrc522[NR_OF_READERS];

/**
   Initialize.
*/
void setup() {

  Serial.begin(9600);           // Initialize serial communications with the PC
  while (!Serial);              // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                  // Init SPI bus




  /* looking for MFRC522 readers */
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
    //mfrc522[reader].PCD_SetAntennaGain(mfrc522[reader].RxGain_max);
  }
}

/*
   Main loop.
*/

void loop() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {

    // Looking for new cards
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial() && current_Status[reader] != "Card_Present") {
      current_Status[reader]= "Card_Present";
      Serial.print(F("Reader "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID in string:"));
      String id=dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println(id);
      Serial.println("Case 1: Find new card");
      Serial.println();
      
      /*Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
        Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));*/
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC..

    else if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial() && current_Status[reader] == "Card_Present"){
      current_Status[reader]= "Card_Present";
      Serial.println("Case 2: Card is till there");
    }
    else if (!mfrc522[reader].PICC_IsNewCardPresent() && current_Status[reader] != "No_Card"){
      current_Status[reader]= "No_Card";
      Serial.println("Case 3: No card");
  }
 }//for(uint8_t reader..
  delay(1000);
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
String dump_byte_array(byte * buffer, byte bufferSize) {
  String id = "";
  for (byte i = 0; i < bufferSize; i++) {
    id = id + String(buffer[i], HEX);
  }
  return id; 
}

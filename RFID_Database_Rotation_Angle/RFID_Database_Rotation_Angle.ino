#include <SPI.h>
#include <MFRC522.h>

// PIN Numbers : RESET + SDAs
#define RST_PIN         9
#define SS_1_PIN        10
#define SS_2_PIN        8
#define SS_3_PIN        7
#define SS_4_PIN        6
#define NR_OF_READERS   4

byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN};

// Create an MFRC522 instance :
MFRC522 mfrc522[NR_OF_READERS];

/**
   Initialize.
*/
void setup() {
  //Database---item and senario
  String left_Item_ID = "";
  String right_Item_ID = "";
  String leftt_Senario_ID = "";
  String right_Senario_ID = "";
  Serial.begin(9600);           // Initialize serial communications with the PC
  while (!Serial);              // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();                  // Init SPI bus
  
  /* looking for MFRC522 readers */
  for (uint8_t reader = 0; reader < NR_OF_READERS; ++reader)
  {
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
    {
      printPiccDetails(reader);
      mfrc522[reader].PICC_HaltA();      // Halt PICC
      mfrc522[reader].PCD_StopCrypto1(); // Stop encryption on PCD
    }
  }

  //Define the old list and new list [left,right]
  int L_R_old[2];
  L_R_old[0] = 0;
  L_R_old[1] = 0;

  int L_R_new[2];
  L_R_new[0] = 0;
  L_R_new[1] = 0;

}

/*
   Main loop.
*/

void loop() {
  uint8_t reader0 = 0;
  uint8_t reader1 = 1;
  uint8_t reader2 = 2;
  uint8_t reader3 = 3;
  
  // Looking for new cards
  
   for (uint8_t reader = 0; reader < NR_OF_READERS; ++reader)
  {
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
    {
      Serial.print(F("Reader "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      String left_Item_ID = dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.print(left_Item_ID);
      Serial.println();

      mfrc522[reader0].PICC_HaltA();      // Halt PICC
      mfrc522[reader0].PCD_StopCrypto1(); // Stop encryption on PCD
    }
  }
  ///////////////////////////////////////////
  //Creat database below:
  String item_Data[][3] = {{"d90f835","20"}, {"C200F811", "60"}};
  String senario_Data[][3] = {{"E800F835", "10"}, {"B200Y811", "30"}};

  //find item score
  for (int i = 0; i < 3; i++) {
    if (item_Data[i][0] == left_Item_ID) {
      leftPlate = item_Data[i][1].toInt();
    }
    if (item_Data[i][0] == right_Item_ID) {
      leftPlate = item_Data[i][1].toInt();
    }
  }//for ends item

  //find senario score
  for (int i = 0; i < 3; i++) {
    if (senario_Data[i][0] == left_Senario_ID) {
      leftSenario = senario_Data[i][1].toInt();
    }
    if (senario_Data[i][0] == right_Senario_ID) {
      rightSenario = senario_Data[i][1].toInt();
    }
  }//for ends senario

  //Add both item score and senario score
  leftTotal = leftPlate + leftSenario;
  rightTotal = rightPlate + rightSenario;

  //Create current left and right score list

  
  L_R_old[0] = L_R_new[0];
  L_R_old[1] = L_R_new[1];


  
  L_R_new[0] = leftTotal;
  L_R_new[1] = rightTotal;


  //Append to the final list
  L1 = L_R_old[0];
  R1 = L_R_old[1];
  L1 = L_R_new[0];
  R2 = L_R_new[1];

  /////////////Rotation and angle start////////////////////
  if (((L1 > R1) or (L1 == R1)) and (L2 < R2)) {
    rotation = 1;
  }
  else if (((L1 < R1) or (L1 == R1)) and (L2 > R2)) {
    rotation = -1;
  }
  else if ((L1 > R1) and ((L2 < R2) or (L2 == R2))) {
    rotation = 1;
  }
  else if ((L1 < R1) and ((L2 > R2) or (L2 == R2))) {
    rotation = -1;
  }
  else if (L1 == R1 and L2 == R2) {
    rotation = 0;
  }
  else if ((L1 > R1) and (L2 > R2)) {
    d1 = abs(L1 - R1);
    d2 = abs(L2 - R2);
    d3 = d1 - d2;
    if (d3 < 0) {
      rotation = -1;
    }
    else if (d3 > 0) {
      rotation = 1;
    }
  }
  else if ((L1 < R1) and (L2 < R2)) {
    d1 = abs(L1 - R1);
    d2 = abs(L2 - R2);
    d3 = d1 - d2;
    if (d3 < 0) {
    rotation = 1;
    }else if (d3 > 0) {
    rotation = -1;
    }
  }

  // Calculate angle and covert to step
  dPrevious = abs(L1 - R1);
  dnew = abs(L2 - R2);

  if (((L1 < R1) or (L1 == R1)) and ((L2 < R2) or (L2 == R2))) {
    angle = round(abs(dPrevious - dnew) * 40 / 105);

  }
  else if ((L1 < R1) and ((L2 > R2))) {
    angle = round(abs(dPrevious + dnew) * 40 / 105);

  }
  else if ((L1 > R1)) and ((L2 > R2)) {
    angle = round(abs(dPrevious - dnew) * 40 / 105);

  }
  else if ((L1 > R1)) and ((L2 < R2)) {
    angle = round(abs(dPrevious + dnew) * 40 / 105);

  }
  ///////////////Rotation and Angle end///////////////////


}//for loop ends


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printPiccDetails(uint8_t &readerNum)
{
  Serial.print(F("Reader "));
  Serial.print(readerNum);
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F(": Card UID:"));
  Serial.println();
}

String getReaderUidByteAsString(uint8_t &readerNum)
{
  return dump_byte_array(mfrc522[readerNum].uid.uidByte, mfrc522[readerNum].uid.size);
}

String dump_byte_array(byte * buffer, byte bufferSize) {
  String id = "";
  for (byte i = 0; i < bufferSize; i++) {
    id = id + String(buffer[i], HEX);
  }
  return id;
}

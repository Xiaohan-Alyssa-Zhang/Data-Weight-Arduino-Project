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

  //Define the old list and new list [left,right)
  int L_R_old[2];
  L_R_old[0]=0
  L_R_old[1]=0

  int L_R_new[2];
  L_R_new[0]=0
  L_R_new[1]=0

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
    // reader0 starts below
    if (mfrc522[reader0].PICC_IsNewCardPresent() && mfrc522[reader0].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader0);

      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      String left_Item_ID=dump_byte_array(mfrc522[reader0].uid.uidByte, mfrc522[reader0].uid.size);
      Serial.print(left_Item_ID);
      Serial.println();
      /*Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
        Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));*/
      // Halt PICC
      mfrc522[reader0].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader0].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC..



    // reader1 starts below
    if (mfrc522[reader1].PICC_IsNewCardPresent() && mfrc522[reader1].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader1);

      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      String right_Item_ID=dump_byte_array(mfrc522[reader1].uid.uidByte, mfrc522[reader1].uid.size);
      Serial.print(right_Item_ID);
      Serial.println();
      /*Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
        Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));*/
      // Halt PICC
      mfrc522[reader1].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader1].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC..


    // reader2 starts below
    if (mfrc522[reader2].PICC_IsNewCardPresent() && mfrc522[reader2].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader2);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      String left_Senario_ID=dump_byte_array(mfrc522[reader2].uid.uidByte, mfrc522[reader2].uid.size);
      Serial.print(left_Senario_ID);
      Serial.println();

      /*Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
        Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));*/
      // Halt PICC
      mfrc522[reader2].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader2].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC..



    
    // reader3 starts below
    if (mfrc522[reader3].PICC_IsNewCardPresent() && mfrc522[reader3].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader3);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      String right_Senario_ID=dump_byte_array(mfrc522[reader3].uid.uidByte, mfrc522[reader3].uid.size);
      Serial.print(right_Senario_ID);
      Serial.println();

      /*Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
        Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));*/
      // Halt PICC
      mfrc522[reader3].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader3].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC..

   ///////////////////////////////////////////
   //Creat database below:
    String item_Data[][3] = {{"D9 00 F8 35","20"},{"C2 00 F8 11","60"},{"A8 02 8H 98","30"},{"","0"}};
    String senario_Data[][3] = {{"E8 00 F8 35","10"},{"B2 00 Y8 11","30"},{"F8 02 8H 98","5"},{"","0"}};
  
    //find item score
    for(int i=0,i<3,i++){
      if(item_Data[i][0]==left_Item_ID){
        leftPlate=item_Data[i][1].toInt()
      }
      if(item_Data[i][0]==right_Item_ID){
        leftPlate=item_Data[i][1].toInt()
      }
    }//for ends item
    
    //find senario score
    for(int i=0,i<3,i++){
      if(senario_Data[i][0]==left_Senario_ID){
        leftSenario=senario_Data[i][1].toInt()
      }
      if(senario_Data[i][0]==right_Senario_ID){
        rightSenario=senario_Data[i][1].toInt()
      }
    }//for ends senario
  
    //Add both item score and senario score
    leftTotal=leftPlate+leftSenario
    rightTotal=rightPlate+rightSenario
  
    //Create current left and right score list
  
    int L_R_old[2];
    L_R_old[0]=L_R_new[0]
    L_R_old[1]=L_R_new[1]
  
    
    int L_R_new[2];
    L_R_new[0]=leftTotal
    L_R_new[1]=rightTotal
  
    
    //Append to the final list
    L1=L_R_old[0]
    R1=L_R_old[1]
    L1=L_R_new[0]
    R2=L_R_new[1]

    /////////////Rotation and angle start////////////////////
    if(((L1>R1)or(L1==R1))and (L2<R2)){
      rotation=1
  }
    else if(((L1<R1)or(L1==R1))and (L2>R2)){
       rotation=-1
  }     
    else if((L1>R1) and ((L2<R2) or(L2==R2))){
      rotation=1
  }        
    else if((L1<R1) and ((L2>R2) or(L2==R2))){
      rotation=-1
  }     
    else if(L1==R1 and L2==R2){
      rotation=0
  }
    else if ((L1>R1)and (L2>R2)){
      d1=abs(L1-R1)
      d2=abs(L2-R2)
      d3=d1-d2
      if (d3<0){
          rotation=-1
      }   
      else if (d3>0){
          rotation=1
      }
  }       
    else if ((L1<R1)and (L2<R2)){
      d1=abs(L1-R1)
      d2=abs(L2-R2)
      d3=d1-d2
      if (d3<0){}
          rotation=1
      }
      else if (d3>0){
          rotation=-1
      }
  }   
  
    dPrevious=abs(L1-R1)
    dnew=abs(L2-R2)
    if(((L1<R1) or(L1==R1)) and ((L2<R2)or(L2==R2))){
      angle=round(abs(dPrevious-dnew)*40/105)
      
  }        
    else if((L1<R1)and ((L2>R2))){
      angle=round(abs(dPrevious+dnew)*40/105)
      
  }       
    else if((L1>R1))and((L2>R2)){
      angle=round(abs(dPrevious-dnew)*40/105)
      
  }        
    else if((L1>R1)) and ((L2<R2)){
      angle=round(abs(dPrevious+dnew)*40/105)
      
  }    
  ///////////////Rotation and Angle end///////////////////
      
  } //for loop ends




/**
   Helper routine to dump a byte array as hex values to Serial.
*/
String dump_byte_array(byte * buffer, byte bufferSize) {
  String id="";
  for (byte i = 0; i < bufferSize; i++) {
//    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
//    buffer[i] < 0x10 ? " 0" : " ";
    id=id+String(buffer[i], HEX);
  }
//  Serial.print("totalID: "+ id);
  return id;
}

#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>


const uint8_t NR_OF_READERS = 4;

const uint8_t RST_PIN  = 49;
const uint8_t SS_1_PIN = 53;
const uint8_t SS_2_PIN = 48;
const uint8_t SS_3_PIN = 46;
const uint8_t SS_4_PIN = 47;


byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN};

MFRC522 mfrc522[NR_OF_READERS];

enum RFID_STATE : byte
{
  CARD_PRESENT,
  NO_CARD,
  UNKNOWN_CARD_STATE
};

RFID_STATE currentState[NR_OF_READERS] = {UNKNOWN_CARD_STATE, UNKNOWN_CARD_STATE, UNKNOWN_CARD_STATE, UNKNOWN_CARD_STATE};



//0:left_item_ID  
String detector[4]={};

//Database
String item_Database[][3] = {{"d3dde98","20"}, {"33ddc59", "60"},{"Empty","0"}};
String senario_Database[][3] = {{"33cc838", "10"}, {"b3558d9", "10"},{"Empty","0"}};

int left_Item_Score     = 0;
int right_Item_Score    = 0;
int left_Senario_Score  = 0;
int right_Senario_Score = 0;
int leftTotal          = 0;
int rightTotal         = 0;

int L_R_old[2];
int L_R_new[2];

int rotation = 0;
int angle = 0;

// Stepper
const int pwmA = 3;
const int pwmB = 11;
const int brakeA = 2;
const int brakeB = 8;
const int dirA = 12;
const int dirB = 13;
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 12, 13);


void setup()
{
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
    delay(4);
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }

  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(brakeA, OUTPUT);
  pinMode(brakeB, OUTPUT);
 
  digitalWrite(pwmA, HIGH);
  digitalWrite(pwmB, HIGH);
  digitalWrite(brakeA, LOW);
  digitalWrite(brakeB, LOW);

  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
}

void loop()
{
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  {
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial() && currentState[reader] != CARD_PRESENT)
    {
      currentState[reader] = CARD_PRESENT;
      Serial.print(F("Reader "));
      Serial.print(reader);
      Serial.print(F(": "));
      detector[reader]=printPiccDetails(reader);
//      mfrc522[reader].PICC_DumpDetailsToSerial(&mfrc522[reader].uid);
      
      Serial.println(detector[reader]);
      mfrc522[reader].PCD_StopCrypto1(); // Stop encryption on PCD
    }
    else if (!mfrc522[reader].PICC_IsNewCardPresent() && currentState[reader] != NO_CARD)
    {
      currentState[reader] = NO_CARD;
      Serial.print(F("Reader "));
      Serial.print(reader);
      detector[reader]="Empty";
      Serial.println(detector[reader]);
      Serial.print(F(": "));
      Serial.println("NO CARD");
    }
  }
  setScores();
//  Serial.println("New LR list");
  Serial.println(detector[0]);
  Serial.println(detector[1]);
  Serial.println(detector[2]);
  Serial.println(detector[3]);
//  Serial.println("////////");
//  Serial.println(L_R_new[0]);
//  Serial.println(L_R_new[1]);
  setBalanceRotation();
  Serial.println("rotation");
  Serial.println(rotation);
  Serial.println("angle");
  Serial.println(angle);
  if(detector[0]!= "Empty" or detector[1]!= "Empty"){
    myStepper.step(rotation*angle);
    }
  delay(4000);
}


String printPiccDetails(uint8_t &readerNum){
  Serial.print(F("Reader "));
  Serial.print(readerNum);
      // Show some details of the PICC (that is: the tag/card)
  Serial.print(F(": Card UID:"));
  String id=dump_byte_array(mfrc522[readerNum].uid.uidByte, mfrc522[readerNum].uid.size);
  return id;
}
  String dump_byte_array(byte * buffer, byte bufferSize) {
  String id = "";
  for (byte i = 0; i < bufferSize; i++) {
    id = id + String(buffer[i], HEX);
  }
  return id;
}


void setScores(){ 
  //find item score
  for (int i = 0; i < 3; i++) {
    
    if (item_Database[i][0] == detector[0]) {
      left_Item_Score = item_Database[i][1].toInt();
      Serial.println("left_Item_Score: ");
      Serial.println(left_Item_Score);
    }  
    if (item_Database[i][0] == detector[1]) {
      right_Item_Score = item_Database[i][1].toInt();
      Serial.println("right_Item_Score: ");
      Serial.println(right_Item_Score);
    } 
  }//for ends item
  
  for (int i = 0; i < 3; i++) {
    if (senario_Database[i][0] == detector[2]) {
      left_Senario_Score = senario_Database[i][1].toInt();
      Serial.println("left_Senario_Score: ");
      Serial.println(left_Senario_Score);
    }
    else if(senario_Database[i][0] == detector[3]) {
      right_Senario_Score = senario_Database[i][1].toInt();
      Serial.println("right_Senario_Score: ");
      Serial.println(right_Senario_Score);
    }
  }//for ends senario
  //Add both item score and senario score

  leftTotal = left_Item_Score + left_Senario_Score;
  rightTotal = right_Item_Score + right_Senario_Score;
  //Exception: if plate == 0, then total = 0
  if(left_Item_Score==0){
    leftTotal=0;
    }
  if(right_Item_Score==0){
    rightTotal=0;
    }

    
  //Create current left and right score list
  L_R_old[0] = L_R_new[0];
  L_R_old[1] = L_R_new[1];
  
  L_R_new[0] = leftTotal;
  L_R_new[1] = rightTotal;

 
// Serial.print(left_Senario_Score);
}



void setBalanceRotation()
{
  int d1, d2, d3, dPrevious, dnew;

  //Append to the final list
  int &L1 = L_R_old[0];
  int &R1 = L_R_old[1];

  int &L2 = L_R_new[0];
  int &R2 = L_R_new[1];

//  Serial.println(L1);
//  Serial.println(R1);
//  Serial.println(L2);
//  Serial.println(R2);

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
  }
  else if (d3 > 0) {
    rotation = -1;
  }
}

  // Calculate angle and covert to step
  dPrevious = abs(L1 - R1);
  dnew = abs(L2 - R2);
  
  if ( ((L1 < R1) or (L1 == R1)) and ((L2 < R2) or (L2 == R2)) ) {
    angle = round(abs(dPrevious - dnew) * 40 / 351);
  }
  else if ((L1 < R1) and (L2 > R2)) {
    angle = round(abs(dPrevious + dnew) * 40 / 351);
  }
  else if ((L1 > R1) and (L2 > R2)) {
    angle = round(abs(dPrevious - dnew) * 40 / 351);
  }
  else if ((L1 > R1) and (L2 < R2)) {
    angle = round(abs(dPrevious + dnew) * 40 / 351);
  }

}

#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>
#include <LiquidCrystal.h>

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
String item_Database[][3] = {{"4425252f46480","10"},{"42dc3aa4d5380","50"},{"924cf42","40"}, {"d51e4748", "10"},{"23d59e80","5"},{"c5064758","50"},{"a5df4558","100"},{"6255fb2","25"},{"Empty","0"}};
String senario_Database[][3] = {{"c37e448", "10"}, {"d343838", "105"},{"6341018","60"},{"F364ed9","85"},{"33c9838","60"},{"134718a","15"},{"a34f709","50"},{"a3576da","90"},{"b3558d9","10"},{"d3dde98","35"},{"33cc838","50"},{"33ddc59","90"},{"Empty","0"}};
//物品： 照片：924cf42 "40"  鞋：d51e4748 "10" 苹果：23d59e80 "5" 票：c5064758 "50" 手机：a5df4558 "100" 牛奶：6255fb2 "25"
//普通苹果：c37e448 "10" 杀人犯苹果：d343838 "105" 社团门票：6341018 "60" 伦敦车票：F364ed9 "85" 签名鞋子：33c9838 "80" 新鞋子：134718a "15" 旅游照片：a34f709 "50" 狗仔照片：a3576da "90" 垃圾场牛奶：b3558d9 "10"  便利贴牛奶：d3dde98 "35" 新手机：33cc838 "50" Jack手机：33ddc59 "90";

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

//Screen 

const int en1 = 23;
const int en2 = 26;
LiquidCrystal lcd1(22, en1, 30, 24, 28, 32);
LiquidCrystal lcd2(22, en2, 30, 24, 28, 32);

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  lcd1.begin(16, 2);
  lcd2.begin(16, 2);

  lcd1.print("Data Value:");
  lcd1.setCursor(0, 1);
  
  lcd2.print("Data Value:");
  lcd2.setCursor(0, 1);

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
//  Serial.println("rotation");
//  Serial.println(rotation);
//  Serial.println("angle");
//  Serial.println(angle);
//  Serial.println("Left Total: ");
//  Serial.print(leftTotal);
//  Serial.println("New List");
//  Serial.println(L_R_new[0]);
//  Serial.println(L_R_new[1]);
//  Serial.print("Old List");
//  Serial.println(L_R_old[0]);
//  Serial.println(L_R_old[1]);
//  lcd1.setCursor(0, 1); 
  lcd1.print(leftTotal);
  lcd2.print(rightTotal);
  
  
  if(L_R_old[0] == L_R_new[0] and L_R_old[1] == L_R_new[1]){
    myStepper.step(0);
    }else{
      myStepper.step(rotation*angle);
    }
  delay(3000);
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
    else if (item_Database[i][0] == detector[1]) {
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
    if(senario_Database[i][0] == detector[3]) {
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
  
  if ( (L1 < R1)and (L2 < R2)){
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
  else if ((L1 == R1)and (L2 == R2)) {
    angle = round(abs(dPrevious - dnew) * 40 / 351);
  }
  else if ( (L1 == R1)and (L2 < R2)) {
    angle = round(abs(dPrevious - dnew) * 40 / 351);
  }
  else if ( (L1 == R1)and (L2 > R2) ) {
    angle = round(abs(dPrevious - dnew) * 40 / 351);
  }
  else if ( (L1 < R1)and (L2 == R2) ) {
    angle = round(abs(dPrevious - dnew) * 40 / 351);
  }
  else if ( (L1 > R1)and (L2 == R2) ) {
    angle = round(abs(dPrevious - dnew) * 40 / 351);
  }
}

#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>

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

String left_Item_ID = "";
String right_Item_ID= "";
String left_Senario_ID= "";
String right_Senario_ID= "";

String item_Database[][3] = {{"33ddc59","20"}, {"33cc838", "60"},{"Empty","0"}};
String senario_Database[][3] = {{"33ddc59", "20"}, {"33cc838", "60"},{"Empty","0"}};

int leftPlateScore     = 0;
int rightPlateScore    = 0;
int leftScenarioScore  = 0;
int rightScenarioScore = 0;
int leftTotal          = 0;
int rightTotal         = 0;


int L_R_old[2];
int L_R_new[2];

int rotation = 0;
int angle = 0;

void setupReaders();

//////////////////////////////////////Motor Steppter/////////////////////////////

const int pwmA = 3;
const int pwmB = 11;
const int brakeA = 2;
const int brakeB = 8;
const int dirA = 12;
const int dirB = 13;
const int stepsPerRevolution = 200;
// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 12, 13);

//////////////////////////////////////Motor Steppter/////////////////////////////



/**
   Initialize.
*/
void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  setupReaders();
  

  
//////////////////////////////////////Motor Steppter/////////////////////////////
// set the PWM and brake pins so that the direction pins  // can be used to control the motor:
 // set the PWM and brake pins so that the direction pins  // can be used to control the motor:
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
  // initialize the serial port:
  
//////////////////////////////////////Motor Steppter/////////////////////////////
}

/*
   Main loop.
*/

void loop() {
    // check readers and grab the uid 
   for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      //call printPiccDetail function to get the each id.
      if(reader==0){
        left_Item_ID=printPiccDetails(reader);
        Serial.print(left_Item_ID);
        Serial.println(); 
      }
      if(reader==1){
        right_Item_ID=printPiccDetails(reader);
        Serial.print(right_Item_ID);
        Serial.println();
       }
       if(reader==2){
        left_Senario_ID=printPiccDetails(reader);
        Serial.print(left_Senario_ID);
        Serial.println();
       }
       if(reader==3){
        right_Senario_ID=printPiccDetails(reader);
        Serial.print(right_Senario_ID);
        Serial.println();
       }
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    }
    if (!mfrc522[reader].PICC_IsNewCardPresent() && !mfrc522[reader].PICC_ReadCardSerial()){
      if(reader==0){
        left_Item_ID="Empty";
        Serial.print(left_Item_ID);
        }
      if(reader==0){
        right_Item_ID="Empty";
        }
      if(reader==0){
        left_Senario_ID="Empty";
        }
      if(reader==0){
        right_Senario_ID="Empty";
      }
    }
 }//reader *4times loop ends here
  // Call setScores function for finding the score of L1 R1 L2 R2.
   setScores();
   setBalanceRotation();
 
//////////////////////////////////////Motor Steppter/////////////////////////////

// step one revolution in one direction:
//Control the movement of stepper motor so that it only starts when passing above code.
//Q2: I'm confused about what should be put in the if() statement?
//  if(finish one comparsion on scale?){
//  myStepper.step(rotation*angle);
//  }
//  Serial.println(leftTotal);
} //for loop ends
//////////////////////////////////////Motor Steppter/////////////////////////////



/**
   Helper routine to dump a byte array as hex values to Serial.
*/

void setupReaders()
{
  SPI.begin();// Init SPI bus
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
    //mfrc522[reader].PCD_SetAntennaGain(mfrc522[reader].RxGain_max);
  }
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
  int leftTotal;
  int rightTotal;
  int left_Item_Score;
  int right_Item_Score;
  int left_Senario_Score;
  int right_Senario_Score;
  //find item score
  for (int i = 0; i < 3; i++) {
    if (item_Database[i][0] == left_Item_ID) {
      left_Item_Score = item_Database[i][1].toInt();
    }
    if (item_Database[i][0] == right_Item_ID) {
      right_Item_Score = item_Database[i][1].toInt();
    }  
    
  }//for ends item

  
  for (int i = 0; i < 3; i++) {
    if (senario_Database[i][0] == left_Senario_ID) {
      left_Senario_Score = senario_Database[i][1].toInt();
    }
    else if(senario_Database[i][0] == right_Senario_ID) {
      right_Senario_Score = senario_Database[i][1].toInt();
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
  int d1, d2, d3, dPrevious, dnew, angle;

  //Append to the final list
  int &L1 = L_R_old[0];
  int &R1 = L_R_old[1];

  int &L2 = L_R_new[0];
  int &R2 = L_R_new[1];

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
    angle = round(abs(dPrevious - dnew) * 40 / 105);
  }
  else if ((L1 < R1) and (L2 > R2)) {
    angle = round(abs(dPrevious + dnew) * 40 / 105);
  }
  else if ((L1 > R1) and (L2 > R2)) {
    angle = round(abs(dPrevious - dnew) * 40 / 105);
  }
  else if ((L1 > R1) and (L2 < R2)) {
    angle = round(abs(dPrevious + dnew) * 40 / 105);
  }
}

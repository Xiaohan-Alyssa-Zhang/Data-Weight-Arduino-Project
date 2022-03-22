/*
    What is this?

    What does it do?

    What do you need to get it to work?

    Pinout:

    | Pin  | Variable   | Function               |
    | ---- | ---------- | ---------------------- |
    | `A0` | `SS_1_PIN` | RFID CS `#1`           |
    | `A1` | `SS_2_PIN` | RFID CS `#2`           |
    | `A2` | `SS_3_PIN` | RFID CS `#3`           |
    | `A3` | `SS_4_PIN` | RFID CS `#4`           |
    | `13` | `SCK`      | RFID SPI Clock         |
    | `12` | `MISO`     | RFID SPI MISO          |
    | `11` | `MOSI`     | RFID SPI MOSI          |
    | `10` | `RST_PIN`  | RFID SPI Reset Pin     |
    | `~9` | `motor1`   | Stepper Motor Pin `#1` |
    | `8`  | `motor2`   | Stepper Motor Pin `#2` |
    | `7 ` | `brakeA`   | Stepper A Brake        |
    | `~6` | `pwmA`     | Stepper A Speed        |
    | `~5` | `dirA`     | Stepper A Direction    |
    | `4 ` | `brakeB`   | Stepper B Brake        |
    | `~3` | `pwmB`     | Stepper B Speed        |
    | `2 ` | `dirB`     | Stepper B Direction    |

*/
#include <SPI.h>
#include <MFRC522.h>
#include <Stepper.h>
//------------------------------------------------------------------------------------------------------------
// Stepper

const uint8_t motor1 = 9;
const uint8_t motor2 = 8;
const uint8_t brakeA = 7;
const uint8_t pwmA   = 6;
const uint8_t dirA   = 5;

const uint8_t brakeB = 4;
const uint8_t pwmB   = 3;
const uint8_t dirB   = 2;

const uint8_t stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, motor1, motor2);
//------------------------------------------------------------------------------------------------------------
// MFRC522
const uint8_t RST_PIN       = 9;
const uint8_t NR_OF_READERS = 4;

const uint8_t ssPins[] = {
  A0,
  A1,
  A2,
  A3
};
MFRC522 mfrc522[NR_OF_READERS];

enum ReaderDataType
{
  left_Item_ID,
  right_Item_ID,
  leftt_Senario_ID,
  right_Senario_ID
};

//------------------------------------------------------------------------------------------------------------
// Items and Scenarios

const uint8_t numberOfItems = 4;

byte itemUids[numberOfItems][4] =
{
  {0xD9, 0x00, 0xF8, 0x35},
  {0xC2, 0x00, 0xF8, 0x11},
  {0xA8, 0x02, 0x8A, 0x98},
  {0x00, 0x00, 0x00, 0x00}
};

byte itemScores[numberOfItems] = {20, 60, 30, 0};

const uint8_t numberOfScenarios = 4;

byte scenarioUids[numberOfScenarios][4] =
{
  {0xE8, 0x00, 0xF8, 0x35},
  {0xB2, 0x00, 0xA8, 0x11},
  {0xF8, 0x02, 0x8A, 0x98},
  {0x00, 0x00, 0x00, 0x00}
};

byte scenarioScores[numberOfScenarios] = {10, 30, 5, 0};

//------------------------------------------------------------------------------------------------------------
// Scores

int L_R_old[2];
int L_R_new[2];

byte leftPlateScore     = 0;
byte rightPlateScore    = 0;
byte leftScenarioScore  = 0;
byte rightScenarioScore = 0;
byte leftTotal          = 0;
byte rightTotal         = 0;
//------------------------------------------------------------------------------------------------------------
// Stepper

int rotation = 0;

void setup()
{
  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial);     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  setupStepper();
  setupReaders();
}

//------------------------------------------------------------------------------------------------------------

void loop()
{
  checkReaders();
  setScores();
  setBalanceRotation();
}

/*
   Functions related to the RFID readers
*/

//------------------------------------------------------------------------------------------------------------

void setupReaders()
{
  SPI.begin();                  // Init SPI bus
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

//------------------------------------------------------------------------------------------------------------

void printPiccDetails(uint8_t &readerNum)
{
  Serial.print(F("Reader "));
  Serial.print(readerNum);
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F(": Card UID:"));
  Serial.print(dump_byte_array(mfrc522[readerNum].uid.uidByte, mfrc522[readerNum].uid.size));
  Serial.println();
}

String getReaderUidByteAsString(uint8_t &readerNum)
{
  return dump_byte_array(mfrc522[readerNum].uid.uidByte, mfrc522[readerNum].uid.size);
}

//------------------------------------------------------------------------------------------------------------

void checkReaders()
{
  for (uint8_t reader = 0; reader < NR_OF_READERS; ++reader)
  {
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
    {
      printPiccDetails(reader);
      mfrc522[reader].PICC_HaltA();      // Halt PICC
      mfrc522[reader].PCD_StopCrypto1(); // Stop encryption on PCD
    }
  }
}

//------------------------------------------------------------------------------------------------------------

String dump_byte_array(byte * buffer, byte bufferSize)
{
  String id = "";
  for (byte i = 0; i < bufferSize; i++) {
    id = id + String(buffer[i], HEX);
  }
  return id;
}

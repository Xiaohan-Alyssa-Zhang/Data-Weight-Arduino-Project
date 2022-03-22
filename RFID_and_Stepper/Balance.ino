/*
   Functions related to the Balance
*/
void setScores()
{
  for (int i = 0; i < numberOfItems; i++)
  {
    uint32_t &currentItemUid = *((uint32_t*)(itemUids[i]));

    if (currentItemUid == *((uint32_t*)mfrc522[left_Item_ID].uid.uidByte))
      leftPlateScore = itemScores[i];

    if (currentItemUid == *((uint32_t*)mfrc522[right_Item_ID].uid.uidByte))
      rightPlateScore = itemScores[i];

    uint32_t &currentScenarioUid = *((uint32_t*)(scenarioUids[i]));

    if (currentScenarioUid == *((uint32_t*)mfrc522[leftt_Senario_ID].uid.uidByte))
      leftScenarioScore = scenarioScores[i];

    if (currentScenarioUid == *((uint32_t*)mfrc522[right_Senario_ID].uid.uidByte))
      rightScenarioScore = scenarioScores[i];

  }

  for (int i = 0; i < numberOfScenarios; i++)
  {
    uint32_t &currentScenarioUid = *((uint32_t*)(scenarioUids[i]));

    if (currentScenarioUid == *((uint32_t*)mfrc522[leftt_Senario_ID].uid.uidByte))
      leftScenarioScore = scenarioScores[i];

    if (currentScenarioUid == *((uint32_t*)mfrc522[right_Senario_ID].uid.uidByte))
      rightScenarioScore = scenarioScores[i];
  }

  leftTotal = leftPlateScore + leftScenarioScore;
  rightTotal = rightPlateScore + rightScenarioScore;

}
//------------------------------------------------------------------------------------------------------------
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
    if (d3 < 0) {}
    rotation = 1;
  }
  else if (d3 > 0) {
    rotation = -1;
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

  myStepper.step(rotation * angle);
}

//------------------------------------------------------------------------------------------------------------

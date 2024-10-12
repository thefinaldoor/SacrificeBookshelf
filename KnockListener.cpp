#include <Arduino.h>
#include "KnockListener.h"

//***********************************************************************
//************************  ListenStart  ********************************
bool KnockListener::listenStart() 
{
  // listen for a knock
  int reading = analogRead(_knockSensorPin);
  if (reading >= _threshold) {
    // we have a valid first knock
    _knockTimer = 0;
    _sequenceTimer = 0;
    _currentKnockNumber = 0;

    // reinitialize the listening array
    for (int i = 0; i < _maximumKnocks; ++i) {
      _knockReadings[i] = 0;
    }    
    return true;
  } else {
    return false;
  }
}

//***********************************************************************
//************************  ListenTick  *********************************
bool KnockListener::listenTick(int timerDelta) 
{
  _knockTimer += timerDelta;
  _sequenceTimer += timerDelta;

  // have we given ourselves enough time for the previous knock to fade?
  if (_knockTimer > _knockFadeTime) {
    //listen for a knock
    int reading = analogRead(_knockSensorPin);  
    if (reading >= _threshold) {
      // we have another knock, so record the time since the previous one
      _knockReadings[_currentKnockNumber++] = _knockTimer;
      _knockTimer = 0;
    }
  }

  // return true if we're done listening for knocks in this sequence
  return (_sequenceTimer > _knockComplete) || (_currentKnockNumber >= _maximumKnocks);
}

//***********************************************************************
//**********************  ValidateKnock  ********************************
bool KnockListener::validateKnock()
{
  int currentKnockCount = 0;
  int maxKnockInterval = 0;                // We use this later to normalize the times.
  
  // count the knocks
  for (int i = 0; i < _maximumKnocks; ++i) {
    if (_knockReadings[i] > 0) {
      currentKnockCount++;
    }

    // collect normalization data while we're looping.
    if (_knockReadings[i] > maxKnockInterval) {
      maxKnockInterval = _knockReadings[i];
    }
  }

  // fail if the count doesn't match the expected number
  if (currentKnockCount != _secretKnockCount) {
    return false; 
  }

  int totaltimeDifferences = 0;
  
  Serial.println();
  Serial.print("Time difference: ");
  
  // Normalize the times
  for (int i = 0; i < _maximumKnocks; ++i) { 
    _knockReadings[i] = map(_knockReadings[i], 0, maxKnockInterval, 0, 100);      
    int timeDiff = abs(_knockReadings[i] - _secretCode[i]);
    Serial.print(timeDiff);
    Serial.print(", ");
    if (timeDiff > _rejectValue){ // Individual value too far out of whack
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  
  // It can also fail if the whole thing is too inaccurate.
  if (totaltimeDifferences / _secretKnockCount > _averageRejectValue){
    return false; 
  }
  
  return true;
}

//***********************************************************************
//*****************  CountKnocksInSecretCode  ***************************
// Run this once during setup
void KnockListener::countKnocksInSecretCode()
{
  for (int i = 0; i < _maximumKnocks; ++i) {
    if (_secretCode[i] > 0) {
      ++_secretKnockCount;
    }
  }  
}


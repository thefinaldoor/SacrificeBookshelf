class KnockListener
{
  // Maximum number of knocks to listen for
  static const int _maximumKnocks = 20;
  
  // listener config values
  int _knockSensorPin;
  int _threshold;
  int _rejectValue;
  int _averageRejectValue;
  int _knockFadeTime;
  int _knockComplete;
  int _secretCode[_maximumKnocks] = {100, 25, 25, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // state variables
  int _knockReadings[_maximumKnocks];   // When someone knocks this array fills with delays between knocks.
  int _secretKnockCount;
  int _currentKnockNumber;

  int _knockTimer;
  int _sequenceTimer;
 
public:
  KnockListener(int pin, int threshold, int reject, int averageReject, int knockFade, int knockComplete) :
    _knockSensorPin(pin), _threshold(threshold), _rejectValue(reject), _averageRejectValue(averageReject),
    _knockFadeTime(knockFade), _knockComplete(knockComplete)
  {
    countKnocksInSecretCode();
  }
    
  bool listenStart();
  bool listenTick(int timerDelta);
  bool validateKnock();

private:
  void countKnocksInSecretCode();
};


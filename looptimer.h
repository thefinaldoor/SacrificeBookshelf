class LoopTimer
{
  int _previousTime;
  int _time;
  
public:
  void start() { _previousTime = _time = millis(); }
  void tick() { _previousTime = _time; _time = millis(); }
  int delta() { return _time - _previousTime; }
};


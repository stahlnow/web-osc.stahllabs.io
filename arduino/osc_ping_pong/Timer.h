class Timer
{
public:

  Timer(unsigned long _period)
  : period(_period), last_time(millis()) {}

  bool shouldRun() {
    unsigned long now = millis();
    if (now - last_time >= period) {
      last_time = now;
      return true;
    }
    return false;
  }

private:
  unsigned long period;
  unsigned long last_time;
};

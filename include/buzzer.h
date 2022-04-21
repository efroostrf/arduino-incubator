#ifndef buzzer_h
  #define buzzer_h

  #include <Arduino.h>
  #include <AsyncDelay.h>
  #include <constants.h>

  struct ALARM {
    boolean isOn;
    int step;
    int maxStep;
    int stepFrequency;
    int startFrequency;
    boolean stopWithoutOff;
  };

  class Buzzer {
    public:
      Buzzer();
      ALARM alarm = { false, 0, 2, 75, 2600, false };
      int pin = PIN_Buzzer;

      void begin();
      void loop();
      void startAlarm();
      void stopAlarm();
      void stopWithoutOff();
      void beep(unsigned int frequency, unsigned long duration);
    private:
      unsigned long _startBeep = 0;
      unsigned long _stopBeep = 0;
      AsyncDelay _alarmStepChanger;
      void _shutup();
      void _beeep();
  };

#endif
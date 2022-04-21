#ifndef COM_h
  #define COM_h

  #include <Arduino.h>
  #include <AsyncDelay.h>
  #include <controller.h>
  #include <settings.h>

  class COM {
    public:
      COM(IncubatorController* controller, IncubatorSettings* settings, int pushInterval, int baundrate);

      int baundrate;
      int pushInterval;
      IncubatorController* controller;
      IncubatorSettings* settings;
      
      void begin();
      void loop();
      void push();
    private:
      AsyncDelay _asyncPushing;
  };
#endif
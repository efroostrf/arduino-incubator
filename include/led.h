#ifndef led_h
  #define led_h

  #include <Arduino.h>
  #include <WS2812.h>
  #include <constants.h>

  struct COLOR_BLINK {
    boolean isOn;
    byte valueBefore;
    byte brightness;
    unsigned long endMillis;
  };

  class LED {
    public:
      LED();
      cRGB color;

      void begin();
      void loop();
      void blink(String color, int ms, int brightness);
    
    private:
      COLOR_BLINK _redBlink = { false, 0, 0, 0 };
      COLOR_BLINK _greenBlink = { false, 0, 0, 0 };
      COLOR_BLINK _blueBlink = { false, 0, 0, 0 };

      boolean _isAnyBlink();
      void _blinkLoop();
  };

#endif
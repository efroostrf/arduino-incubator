#ifndef settings_h
  #define settings_h

  #include <Arduino.h>
  #include <EEPROM.h>
  #include <RTClib.h>
  #include <AsyncDelay.h>

  struct _ManualModeStruct {
    int temperature; // Температура
    int humidity; // Влажность
    int ventingCount; // Количество вентиляций
    int ventingTime; // Длительность каждой вентиляции в с
  };

  struct _SecondarySettings {
    boolean buzzerAtNight;
    boolean blinkAtNight;
    int displayTimeout;
    int emptyWater;
    int fullWater;
  };

  class IncubatorSettings {
    public:
      IncubatorSettings(boolean loadFromEEPROM);
      
      _ManualModeStruct ManualMode = { 0, 0, 0, 0 };
      _SecondarySettings secondary = { false, false, 60, 570, 372 };
      RTC_DS1307 rtc; // Модуль времени 

      boolean isAuto = false;
      boolean pause = false;
      unsigned long unixIncubationStart = 0;
      unsigned long unixLastVenting = 0;
      int incubationDay = 0;

      void load();
      void save();
      float getTemperature();
      float getTrueTemperature();  // Отличие от getTemperature заключается в том, что возвращается float делённый на 10
      void setTemperature(float newTemperature);
      int getHumidity();
      int dayOfIncubation();
      int ventingDuration();
      int ventingCount();
      boolean isVentingDay();
      boolean isStartVenting();
      boolean isNowVentingEnabled();
      void updateVentingToNow();
      unsigned long ventingDelay();
      void updateDay();
      void resetDayToNow();
      AsyncDelay updateIncubationDay;

    private:
      int _incubationDayForProgram();
      void _loadDatas();
      void _saveDatas();
      void _loadManual();
      void _saveManual();
      void _loadSecondary();
      void _saveSecondary();
  };
#endif
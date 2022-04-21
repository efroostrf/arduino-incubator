#ifndef controller_h
  #define controller_h

  #include <Arduino.h>
  #include <Adafruit_Si7021.h>
  #include <AsyncDelay.h>
  #include <constants.h>
  #include <settings.h>
  #include <buzzer.h>
  #include <led.h>

  struct EmergencySituations {
    boolean preOverheat;
    boolean overheat;
    boolean preHypothermia;
    boolean hypothermia;
  };

  struct RELAY {
    boolean status; // Сигнал на включение
    boolean isAllowed; // Разрешено ли включение
    boolean isReversed; // Низкоуровневое реле
    int pin;
    boolean calledByEmergency;
  };

  struct SENSORS_DATA {
    float temperature;
    int humidity;
    int light;
    int water;
    int waterDryValue;
  };

  class IncubatorController {
    public:
      IncubatorController(IncubatorSettings* setSettings, Buzzer* setBuzzer, LED* setLed);

      IncubatorSettings* settings;
      Buzzer* buzzer;
      LED* led;
      
      Adafruit_Si7021 sensor = Adafruit_Si7021(); // Датчик температуры и влаги

      RELAY heaterRelay = { false, true, false, PIN_RelayHeater, false };
      RELAY hydrationRelay = { false, true, true, PIN_RelayHydration, false };
      RELAY ventingRelay = { false, true, true, PIN_RelayVenting, false };
      SENSORS_DATA sensors = { 0.0, 0, 0, 0, 0 };
      EmergencySituations emergency = { false, false, false, false };

      void begin();
      void loop();
      void updateRelaySignals(); // Обновление релюшек
      boolean heaterLightCheck(); // Есть ли свет от нагревателя (при лампочке накаливания)
    
    private:
      AsyncDelay _emergencyLedBlinking;
      AsyncDelay _emergencyLightBuzzer;
      void _updateSensors(); // Обновление данных от сенсоров в массиве класса
      void _setupRelays(); // Выполняется при begin устанавливая pinOut'ы
      boolean _isRelayEnabled(RELAY relay); // Проверяет, включено ли реле и можно ли включить
      void _updateVenting();
      void _changeRelayState(RELAY relay, boolean state); // Изменяет сигнал на релюшку
      int _calculateWaterPercent(int lightSensorValue); // Рассчитывает процент воды
      void _emergencyCheck(); // Проверяет чрезвычайные ситуации в инкубировании
      void _emergencyHandler(); // Обрабатывает чрезвычайные ситуации
      void _emergencyBlink();
  };
#endif
#include <Arduino.h>
#include <EEPROM.h>
#include <GyverRelay.h>
#include <AsyncDelay.h>
#include <settings.h>
#include <constants.h>
#include <controller.h>
#include <buzzer.h>
#include <led.h>

/*
  Почему регулятор реле тут? Скажете я идиот?
  Я пытался его внедрить в класс IncubatorController,
  но тот никак не поддавался моим попыткам.

  Даже в controller.h не получается, потому что
  появляется "дубликат" с классом ESP8266.
  Бред какой-то. Будет тут.
*/

GyverRelay heatingRegulator(REVERSE);
GyverRelay hydrationRegulator(REVERSE);

IncubatorController::IncubatorController(IncubatorSettings* setSettings, Buzzer* setBuzzer, LED* setLed) {
  settings = setSettings;
  buzzer = setBuzzer;
  led = setLed;
}

void IncubatorController::begin() {
  if (!settings->rtc.begin())     // RTC настроек мы запускаем тут, чтобы не создавать 
    Serial.println("RTC ERROR");  // отдельный метод для IncubatorSettings и не вызывать его в setup.
  settings->updateIncubationDay.start(60000, AsyncDelay::MILLIS); // С интервалом обновления дня то же самое
  settings->updateIncubationDay.expire();
  settings->load();

  sensor.begin();
  _emergencyLedBlinking.start(5000, AsyncDelay::MILLIS);
  _emergencyLightBuzzer.start(3000, AsyncDelay::MILLIS);

  pinMode(PIN_Light, INPUT);
  pinMode(PIN_Water, INPUT);
  pinMode(PIN_IsPowered, INPUT);
  
  _setupRelays();
}

void IncubatorController::_setupRelays() {
  pinMode(heaterRelay.pin, OUTPUT);
  pinMode(hydrationRelay.pin, OUTPUT);
  pinMode(ventingRelay.pin, OUTPUT);
  updateRelaySignals();
}

void IncubatorController::loop() {
  settings->updateDay();
  _updateSensors();
  _updateVenting();
  _emergencyCheck();
  
  if (_emergencyLedBlinking.isExpired()) {
    _emergencyBlink();
    _emergencyLedBlinking.repeat();
  }

  if (!settings->pause) {
    heatingRegulator.input = sensors.temperature;
    heatingRegulator.setpoint = settings->getTemperature();
    heatingRegulator.hysteresis = 0.1;
    heatingRegulator.k = 0.1;

    if (heaterRelay.status != heatingRegulator.getResult()) {
      heaterRelay.status = heatingRegulator.getResult();

      // if (heaterRelay.status) buzzer->beep(1000, 100);
      // else buzzer->beep(800, 100);

      // _emergencyLightBuzzer.restart();
    }

    hydrationRegulator.input = sensors.humidity;
    hydrationRegulator.setpoint = settings->getHumidity();
    hydrationRegulator.hysteresis = 2;
    hydrationRelay.status = hydrationRegulator.getResult();
  } else {
    heaterRelay.status = false;
    hydrationRelay.status = false;
  }

  // if (_emergencyLightBuzzer.isExpired()) {
  //   if (heaterLightCheck() != heaterRelay.status) {
  //     if (heaterRelay.status) buzzer->beep(1000, 100);
  //     else buzzer->beep(800, 100);

  //     // led->blink("r", 100, 10);
  //   }
  //   _emergencyLightBuzzer.repeat();
  // }
  
  if (ventingRelay.status) heaterRelay.status = false; // Если включена вентиляция - обогреватель выключается
  
  _emergencyHandler();
  updateRelaySignals();
}

void IncubatorController::updateRelaySignals() {
  if (_isRelayEnabled(heaterRelay)) 
    _changeRelayState(heaterRelay, true);
  else 
    _changeRelayState(heaterRelay, false);

  if (_isRelayEnabled(hydrationRelay)) 
    _changeRelayState(hydrationRelay, true);
  else 
    _changeRelayState(hydrationRelay, false);

  if (_isRelayEnabled(ventingRelay)) 
    _changeRelayState(ventingRelay, true);
  else 
    _changeRelayState(ventingRelay, false);
}

boolean IncubatorController::heaterLightCheck() {
  if (sensors.light < 400) return true;
  return false;
}

void IncubatorController::_updateSensors() {
  sensors.temperature = sensor.readTemperature();
  sensors.humidity = sensor.readHumidity();
  sensors.light = analogRead(PIN_Light);
  sensors.waterDryValue = analogRead(PIN_Water);
  sensors.water = _calculateWaterPercent(sensors.waterDryValue);
}

boolean IncubatorController::_isRelayEnabled(RELAY relay) {
  if (relay.status && (relay.isAllowed || relay.calledByEmergency)) return true;
  return false;
}

void IncubatorController::_updateVenting() {
  if (!settings->isVentingDay()) return;

  if (settings->isStartVenting()) {
    // Запуск вентилирования

    settings->updateVentingToNow(); // Обновляем время начала вентилирования
  }

  /*
    Не допускаем глубокого охлаждения (ниже 28 градусов на яйце)
    Просто отсекаем вентиляцию при таких низких значениях.

    К слову, чтобы не потерять, нагрев до программных значений после вентиляции
    не должен длиться дольше 30 МИНУТ, поэтому проверяйте это собственноручно.
    Иными словами - обогрев после вентилирования должен быть быстрым.

    TO DO
    Сделано так "колхозно" на случай ЧП.
    В реальности же при таком ЧП на пороге 29 градусов реле начнут плохо щёлкать,
    поэтому нужно сделать выдержку по набору температуры после срабатывания ЧП.
  */
 
  if (sensors.temperature > 29.0) {
    ventingRelay.status = settings->isNowVentingEnabled();
  } else if (ventingRelay.status) {
    ventingRelay.status = false;
  }
}

void IncubatorController::_changeRelayState(RELAY relay, boolean state) {
  if (relay.isReversed) {
    if (state) digitalWrite(relay.pin, LOW);
    else digitalWrite(relay.pin, HIGH);
  } else {
    if (state) digitalWrite(relay.pin, HIGH);
    else digitalWrite(relay.pin, LOW);
  }
}

int IncubatorController::_calculateWaterPercent(int waterValue) {
  int emptyCapacity = settings->secondary.emptyWater;
  int fullCapacity = settings->secondary.fullWater;
  
  int capacityDifference = emptyCapacity - fullCapacity;
  float x = ((float) waterValue - (float) fullCapacity) 
              / (float) capacityDifference;
  
  x = (1.0 - x) * 100.0;

  if (x < 0.0)  x = 0.0;

  return (int) x;
}

void IncubatorController::_emergencyCheck() {
  if (_isRelayEnabled(heaterRelay) && !heaterLightCheck()) {
    // Не работает лампочка!
    buzzer->startAlarm();
  }

  if (digitalRead(PIN_IsPowered) == LOW) {
    // buzzer->startAlarm();
  }

  // Близится перегрев яиц! Постараемся его избежать
  if (sensors.temperature > settings->getTemperature() + 0.2) emergency.preOverheat = true;
  else if (sensors.temperature < settings->getTemperature() + 0.1) emergency.preOverheat = false;

  // Ув-ва, перегрев яиц! Срочно включаем сброс воздуха!
  if (sensors.temperature > settings->getTemperature() + 0.5) emergency.overheat = true;
  else if (sensors.temperature < settings->getTemperature() + 0.2) emergency.overheat = false;
  
  // Близится переохлаждение яиц! Постараемся его избежать
  if (sensors.temperature < settings->getTemperature() - 0.1) emergency.preHypothermia = true;
  else if (sensors.temperature > settings->getTemperature() + 0.05) emergency.preHypothermia = false;

  // // Похоже может случится переохлаждение, давай это исправлять
  // if (sensors.temperature < settings->getTemperature() - 0.3) emergency.hypothermia = true;
  // else if (sensors.temperature > settings->getTemperature() - 0.1) emergency.hypothermia = false;
}

void IncubatorController::_emergencyHandler() {
  if (emergency.preOverheat) {
    // Включаем вентиляцию при перегреве
    if (emergency.overheat) {
      ventingRelay.status = true; 
      ventingRelay.calledByEmergency = true;
    } else {
      if (ventingRelay.calledByEmergency) {
        ventingRelay.status = false;
        ventingRelay.calledByEmergency = false;
      }
    }

    // Если у нас только начинается перегрев - включаем увлажнитель (я думаю это плохая практика, но что поделать)
    hydrationRelay.status = true;
    hydrationRelay.calledByEmergency = true;
  } else {
    if (hydrationRelay.calledByEmergency) {
      hydrationRelay.status = false;
      hydrationRelay.calledByEmergency = false;
    }
  }

  if (emergency.preHypothermia) {
    if (emergency.hypothermia) {
      // Мы не можем ничего больше сделать, только сирена и надежда на человека...
    }

    // Чаще всего такой вариант развивается при активном увлажнении, поэтому временно выключаем его
    hydrationRelay.status = false;
    hydrationRelay.calledByEmergency = true;
    // heaterRelay.status = true;
  } else {
    if (hydrationRelay.calledByEmergency) hydrationRelay.calledByEmergency = false;
  }
}

void IncubatorController::_emergencyBlink() {
  if (emergency.preHypothermia
      || emergency.preOverheat) 
  {
    led->blink("r", 100, 42);
    led->blink("g", 100, 30);
  }
}
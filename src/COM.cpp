#include <COM.h>

COM::COM(IncubatorController* newController, IncubatorSettings* newSettings, int interval = 5000, int setBaundrate = 9600) {
  baundrate = setBaundrate;
  controller = newController;
  settings = newSettings;
  pushInterval = interval;
}

void COM::begin() {
  Serial.begin(baundrate);
  _asyncPushing.start(pushInterval, AsyncDelay::MILLIS);
}

void COM::loop() {
  if (Serial && 
      _asyncPushing.isExpired()) 
  {
    push();
    _asyncPushing.repeat();
  }
}

void COM::push() {
  /*
    Данная функция отправляет данные в сериал в таком формате:

    Температура, Температура настр., влажность, влажность настр.
  */

  Serial.print(settings->incubationDay);
  Serial.print(" ");

  Serial.print(controller->sensors.temperature);
  Serial.print(" ");
  Serial.print(settings->getTemperature());
  Serial.print(" ");
  Serial.print(controller->sensors.humidity);
  Serial.print(" ");
  Serial.print(settings->getHumidity());
  Serial.print(" ");
  Serial.print(controller->sensors.water);

  Serial.print(" ");
  Serial.print(controller->heaterRelay.status);
  Serial.print(" ");
  Serial.print(controller->hydrationRelay.status);
  Serial.print(" ");
  Serial.print(controller->ventingRelay.status);

  Serial.print(" ");
  Serial.print(controller->emergency.preOverheat);
  Serial.print(" ");
  Serial.print(controller->emergency.preHypothermia);
  Serial.print(" ");
  Serial.print(controller->emergency.overheat);
  Serial.print(" ");
  Serial.print(controller->emergency.hypothermia);

  Serial.println();
}
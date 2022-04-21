#include <Arduino.h>
#include <GyverButton.h>
#include <constants.h>
#include <display.h>
#include <settings.h>
#include <programs.h>
#include <controller.h>
#include <led.h>
#include <buzzer.h>
// #include <COM.h>
#include <avr/wdt.h>

GButton bLeft;
GButton bRight;
GButton bUp;
GButton bDown;

LED led;
Buzzer buzzer;
IncubatorSettings settings(false);
IncubatorController controller(&settings, &buzzer, &led);
IncubatorDisplay display(&settings, &controller);
// COM comPlugin(&controller, &settings, 4000, COM_BAUNDRATE);

void initPins() {
  pinMode(PIN_Buttons, INPUT);
  pinMode(PIN_BatteryVoltage, INPUT);
}

void setup() {
  Serial.begin(9600);

  initPins();
  delay(10);

  led.begin();
  buzzer.begin();
  controller.begin();
  display.begin();
  // comPlugin.begin();

  led.blink("g", 50, 20);
  
  // display.disableTimeout(); // Экран будет включён всегда
  // settings.rtc.adjust(DateTime(2022, 4, 9, 13, 22, 0));

  bLeft.setType(LOW_PULL);
  bRight.setType(LOW_PULL);
  bUp.setType(LOW_PULL);
  bDown.setType(LOW_PULL);

  wdt_enable(WDTO_8S); // Детекция зависания с помошью WatchDog
  delay(10);
} 

void buttonsTick() {
  int analogOfButtons = analogRead(PIN_Buttons);

  bLeft.tick(analogOfButtons < 223 && analogOfButtons > 183);
  bRight.tick(analogOfButtons < 402 && analogOfButtons > 362);
  bUp.tick(analogOfButtons < 693 && analogOfButtons > 653);
  bDown.tick(analogOfButtons < 1043 && analogOfButtons > 1003);
}

void loop() {
  buttonsTick();

  if (bUp.isClick()) {
    if (buzzer.alarm.isOn) buzzer.stopWithoutOff();
  }

  display.buttonHoldEvents(bLeft.isHold(), bRight.isHold(), bUp.isHold(), bDown.isHold());
  display.buttonDoubleEvents(bLeft.isDouble(), bRight.isDouble(), bUp.isDouble(), bDown.isDouble());
  display.buttonSingleEvents(bLeft.isSingle(), bRight.isSingle(), bUp.isSingle(), bDown.isSingle());

  controller.loop();
  display.loop();

  // if (controller.heaterRelay.status) {
  //   led.color.b = 5;
  // } else {
  //   led.color.b = 0;
  // }

  buzzer.loop();
  led.loop();
  // comPlugin.loop();

  wdt_reset(); // Обновление таймера WatchDog
}
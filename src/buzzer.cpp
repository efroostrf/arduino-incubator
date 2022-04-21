#include <Arduino.h>
#include <AsyncDelay.h>
#include <buzzer.h>

Buzzer::Buzzer() {}

void Buzzer::begin() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);

  _alarmStepChanger.start(500, AsyncDelay::MILLIS);
}

void Buzzer::loop() {
  if (alarm.isOn
      && _alarmStepChanger.isExpired()) 
  {
    if (!alarm.stopWithoutOff) tone(pin, alarm.startFrequency + alarm.step * alarm.stepFrequency);
    // else _shutup();
    else _beeep();

    if (alarm.step < alarm.maxStep) alarm.step++;
    else alarm.step = 0;

    _alarmStepChanger.repeat();
  } else {
    _beeep();
  }
}

void Buzzer::_beeep() {
  if (millis() > _stopBeep
      && _startBeep != 0
      && _stopBeep != 0)
  {
    _shutup();
    _startBeep = 0;
    _stopBeep = 0;
  }
}

void Buzzer::startAlarm() {
  if (!alarm.isOn) {
    alarm.isOn = true;
    _alarmStepChanger.expire();
  }
}

void Buzzer::stopAlarm() {
  if (alarm.isOn) {
    alarm.isOn = false;
    _shutup();
  }
}

void Buzzer::stopWithoutOff() {
  if (!alarm.stopWithoutOff) {
    alarm.stopWithoutOff = true;
    _shutup();
  }
}

void Buzzer::beep(unsigned int frequency, unsigned long duration) {
  _startBeep = millis();
  _stopBeep = millis() + duration;
  tone(pin, frequency);
}

void Buzzer::_shutup() {
  noTone(pin);
  digitalWrite(pin, LOW);
}
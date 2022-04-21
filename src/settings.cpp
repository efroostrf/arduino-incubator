#include <Arduino.h>
#include <EEPROM.h>
#include <settings.h>
#include <programs.h>
#include <constants.h>

IncubatorSettings::IncubatorSettings(boolean loadFromEEPROM = true) {
  if (loadFromEEPROM) load();
}

void IncubatorSettings::load() {
  _loadDatas();
  _loadManual();
  _loadSecondary();
}

void IncubatorSettings::save() {
  _saveDatas();
  _saveManual();
  _saveSecondary();
}

float IncubatorSettings::getTemperature() {
  if (!isAuto) return getTrueTemperature();
  else return (float) pgm_read_float( &chickensProgram[_incubationDayForProgram()].temperature );
}

float IncubatorSettings::getTrueTemperature() {
  return (float) ManualMode.temperature / 10.0;
}

void IncubatorSettings::setTemperature(float newTemperature) {
  ManualMode.temperature = (float) (newTemperature * 10);
}

int IncubatorSettings::getHumidity() {
  if (!isAuto) return ManualMode.humidity;
  else return (int) pgm_read_byte( &chickensProgram[_incubationDayForProgram()].humidity );
}

int IncubatorSettings::dayOfIncubation() {
  DateTime now = rtc.now();
  unsigned long unixNow = now.unixtime();
  unsigned long difference = (unsigned long) unixNow - (unsigned long) unixIncubationStart;
  unsigned long days = (unsigned long) difference / (unsigned long) 86400;

  if (days > 60) days = 60;

  return (int) days;
}

int IncubatorSettings::ventingDuration() {
  return (int) pgm_read_byte( &chickensProgram[_incubationDayForProgram()].ventingTime ) * (int) 60;
}

int IncubatorSettings::ventingCount() {
  return (int) pgm_read_byte( &chickensProgram[_incubationDayForProgram()].ventingCount );
}

boolean IncubatorSettings::isVentingDay() {
  if (ventingCount() == 0) return false;
  
  return true;
}

boolean IncubatorSettings::isStartVenting() {
  DateTime now = rtc.now();
  unsigned long unixNow = now.unixtime();

  // Тут нужен не ventingDuration, а количество секунд в дне делённое на количество проветриваний да, вот так вот
  if (unixNow > unixLastVenting + ventingDelay()) return true;
  else return false;
}

boolean IncubatorSettings::isNowVentingEnabled() {
  DateTime now = rtc.now();
  unsigned long unixNow = now.unixtime();

  if (unixNow < unixLastVenting + ventingDuration()) return true;
  else return false;
}

void IncubatorSettings::updateVentingToNow() {
  DateTime now = rtc.now();
  unsigned long unixNow = now.unixtime();

  unixLastVenting = unixNow;
  _saveDatas();
}

unsigned long IncubatorSettings::ventingDelay() {
  return (unsigned long) 86400 / (unsigned long) ventingCount();
}

void IncubatorSettings::updateDay() {
  if (updateIncubationDay.isExpired()) {
    incubationDay = dayOfIncubation();
    updateIncubationDay.repeat();
  }
}

void IncubatorSettings::resetDayToNow() {
  DateTime now = rtc.now();
  unixIncubationStart = now.unixtime();
  _saveDatas();
  updateIncubationDay.expire();
  updateDay();
}

int IncubatorSettings::_incubationDayForProgram() {
  int day = incubationDay;
  if (day > 20) day = 20;

  return day;
}

void IncubatorSettings::_loadDatas() {
  EEPROM.get(EEPROM_SAVE_MODE, isAuto);
  EEPROM.get(EEPROM_SAVE_PAUSE, pause);
  EEPROM.get(EEPROM_SAVE_STARTTIME, unixIncubationStart);
  EEPROM.get(EEPROM_SAVE_VENTING, unixLastVenting);
}

void IncubatorSettings::_saveDatas() {
  EEPROM.put(EEPROM_SAVE_MODE, isAuto);
  EEPROM.put(EEPROM_SAVE_PAUSE, pause);
  EEPROM.put(EEPROM_SAVE_STARTTIME, unixIncubationStart);
  EEPROM.put(EEPROM_SAVE_VENTING, unixLastVenting);
}

void IncubatorSettings::_loadManual() {
  EEPROM.get(EEPROM_MANUAL_STRUCT, ManualMode);
}

void IncubatorSettings::_saveManual() {
  EEPROM.put(EEPROM_MANUAL_STRUCT, ManualMode);
}

void IncubatorSettings::_loadSecondary() {
  EEPROM.get(EEPROM_SECONDARY_STRUCT, secondary);
}

void IncubatorSettings::_saveSecondary() {
  EEPROM.put(EEPROM_SECONDARY_STRUCT, secondary);
}
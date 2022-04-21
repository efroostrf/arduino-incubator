#include <Arduino.h>
#include <display.h>

void IncubatorDisplay::_changeCurrentScreen(int newScreen, int curPos = 0) {
  currentMenu = newScreen;
  cursorPosition = curPos;
}

void IncubatorDisplay::goToMainScreen() {
  _changeCurrentScreen(0);
}

void IncubatorDisplay::_defaultCursor_Controls(int maxY, boolean holdingChange = true) {
  if (_isDownSingle
      || (_isDownHold && holdingChange)) 
  {
    if (cursorPosition < maxY) cursorPosition++;
  }

  else if (_isUpSingle
      || (_isUpHold && holdingChange)) 
  {
    if (cursorPosition > 0) cursorPosition--;
  }
}

void IncubatorDisplay::mainScreen() {
  oled.setScale(2);
  oled.invertText(false);

  oled.print(F("Темп. "));
  oled.print(controller->sensors.temperature, 1);
  // oled.print(F("*C"));
  
  oled.setCursor(0, 2);
  oled.print("Влаж. ");
  oled.print(controller->sensors.humidity);
  oled.print(F("%"));
  
  oled.setScale(1);
  oled.setCursor(0, 4);
  oled.print(settings->getTemperature());
  oled.print(F("*C / "));
  oled.print(settings->getHumidity());
  oled.print(F(" %"));

  oled.setCursor(0, 5);
  oled.print(F("Вода: "));
  oled.print(controller->sensors.water);
  oled.print(F("%"));

  oled.setCursor(0, 6);
  oled.print(F("День: "));
  oled.print(settings->incubationDay + 1);

  DateTime now = settings->rtc.now();

  oled.setCursor(0, 7);

  oled.print(now.hour());
  oled.print(":");
  oled.print(now.minute());
  oled.print("  ");

  // // analogReference(INTERNAL);
  float Vbat = (analogRead(PIN_BatteryVoltage) * 5.0) / 1023;
  float del = 0.2428; // R2/(R1+R2)  0.99кОм / (9.88кОм + 0.99кОм)
  float Vin = Vbat / del;
  

  // уровень заряда в процентах
  // максимальный заряд аккумулятора
  float max_v = 4.25; 
  // минимальный заряд аккумулятора
  float min_v = 2.75; 
  int proc = ((Vin - min_v) / (max_v - min_v)) * 100;
  // вывод данных в монитор порта
  // Serial.println(Vin);
  // Serial.println(proc);

  oled.print(Vin);
  oled.print(F("V"));
  oled.print(" ");
  oled.print(proc);
  oled.print(F("%"));

  // analogReference(DEFAULT);
}

void IncubatorDisplay::mainScreen_Controls() {
  if (_isUpSingle) _changeCurrentScreen(1);
  
  _resetButtons();
}

void IncubatorDisplay::selectCategoryScreen() {
  printText(F("Инкубация"), 0, 0, isCurrentCursor(0), 2);
  printText(F("Настройки"), 0, 1, isCurrentCursor(1), 2);
  printText(F("Тест"), 0, 2, isCurrentCursor(2), 2);
}

void IncubatorDisplay::selectCategoryScreen_Controls() {
  if (_isLeftSingle) goToMainScreen();
  _defaultCursor_Controls(2);

  if (_isRightSingle) _changeCurrentScreen(2 + cursorPosition);

  _resetButtons();
}

void IncubatorDisplay::incubationCategory() {
  printText(F("Статус инкуб.: "), 0, 0, isCurrentCursor(0), 1);
  _printBooleanAsText(!settings->pause);

  if (settings->isAuto) {
    printText(F("Инкубация: авто"), 0, 1, isCurrentCursor(1), 1);
    printText(F("Перезапустить"), 0, 2, isCurrentCursor(2), 1);

    printText(F("< Отмотка дней: "), 0, 3, isCurrentCursor(3), 1);
    oled.print(settings->incubationDay);
    oled.print(F(" >"));
  } else {
    printText(F("Инкубация: ручная"), 0, 1, isCurrentCursor(1), 1);
    printText(F("Температура: "), 0, 2, isCurrentCursor(2), 1);
    oled.print(settings->getTrueTemperature());

    printText(F("Влажность: "), 0, 3, isCurrentCursor(3), 1);
    oled.print(settings->ManualMode.humidity);

    printText(F("Кол. вентиляций: "), 0, 4, isCurrentCursor(4), 1);
    oled.print(settings->ManualMode.ventingCount);

    printText(F("Время вент.: "), 0, 5, isCurrentCursor(5), 1);
    oled.print(settings->ManualMode.ventingTime);

    printText(F("Сбросить"), 0, 6, isCurrentCursor(6), 1);
  }
}

void IncubatorDisplay::incubationCategory_Controls() {
  if (_isLeftDouble) {
    goToMainScreen();
    settings->save();
  }

  if (_isLeftSingle) {
    _changeCurrentScreen(1);
    settings->save();
  }

  if (settings->isAuto) _defaultCursor_Controls(3);
  else _defaultCursor_Controls(6);

  if (cursorPosition == 0 && _isRightSingle) settings->pause = !settings->pause;
  if (cursorPosition == 1 && _isRightSingle) settings->isAuto = !settings->isAuto;

  if (!settings->isAuto) {
    if (_isLeftHold) {
      if (cursorPosition == 2) settings->ManualMode.temperature -= 1;
      if (cursorPosition == 3) settings->ManualMode.humidity -= 1;
      if (cursorPosition == 4) settings->ManualMode.ventingCount -= 1;
      if (cursorPosition == 5) settings->ManualMode.ventingTime -= 1;
    }

    if (_isRightHold) {
      if (cursorPosition == 2) settings->ManualMode.temperature += 1;
      if (cursorPosition == 3) settings->ManualMode.humidity += 1;
      if (cursorPosition == 4) settings->ManualMode.ventingCount += 1;
      if (cursorPosition == 5) settings->ManualMode.ventingTime += 1;
    }

    if (_isRightSingle) {
      if (cursorPosition == 6) { // Сброс настроек ручного режима
        settings->setTemperature(37.5);
        settings->ManualMode.humidity = 60;
        settings->ManualMode.ventingCount = 0;
        settings->ManualMode.ventingTime = 0;
      }
    }
  } else {
    if (_isRightSingle) {
      if (cursorPosition == 2) settings->resetDayToNow();
    }
    if (cursorPosition == 3) {
      // if (_isLeftDouble) progDay--;
      // if (_isRightDouble) progDay++;
    }
  }

  _resetButtons();
}

void IncubatorDisplay::settingsCategory() {
  printText(F("Звук ночью: "), 0, 0, isCurrentCursor(0), 1);
  _printBooleanAsText(settings->secondary.buzzerAtNight);

  printText(F("Мигания ночью: "), 0, 1, isCurrentCursor(1), 1);
  _printBooleanAsText(settings->secondary.blinkAtNight);

  printText(F("Таймаут. экрана: "), 0, 2, isCurrentCursor(2), 1);
  oled.print(settings->secondary.displayTimeout);
  oled.print("c");

  printText(F("Вода 0%: "), 0, 3, isCurrentCursor(3), 1);
  oled.print(settings->secondary.emptyWater);

  printText(F("Вода 100%: "), 0, 4, isCurrentCursor(4), 1);
  oled.print(settings->secondary.fullWater);
}

void IncubatorDisplay::settingsCategory_Controls() {
  if (_isLeftDouble) {
    goToMainScreen();
    settings->save();
  }
  
  if (_isLeftSingle) {
    _changeCurrentScreen(1, 1);
    settings->save();
  }

  _defaultCursor_Controls(4);
  
  if (_isRightSingle) {
    if (cursorPosition == 0) settings->secondary.buzzerAtNight = !settings->secondary.buzzerAtNight;
    if (cursorPosition == 1) settings->secondary.blinkAtNight = !settings->secondary.blinkAtNight;
    if (cursorPosition == 3) settings->secondary.emptyWater = controller->sensors.waterDryValue;
    if (cursorPosition == 4) settings->secondary.fullWater = controller->sensors.waterDryValue;
  }

  if (cursorPosition == 2) {
    if (_isLeftHold && settings->secondary.displayTimeout > 10) settings->secondary.displayTimeout -= 1;
    if (_isRightHold) settings->secondary.displayTimeout += 1;
  }

  _resetButtons();
}

void IncubatorDisplay::testingCategory() {
  if (false == true) {
    oled.invertText(false);
    oled.setScale(1);
    oled.autoPrintln(true);
    oled.println(F("Отключите инкубацию чтобы провести тесты."));
    
    oled.fastLineH(24, 0, 128);

    printText("Отключить", 8, 2, true, 2);
  } else {
    printText(F("Сирена 5 секунд"), 0, 0, isCurrentCursor(0), 1);
    printText(F("Обогреватель"), 0, 1, isCurrentCursor(1), 1);
    printText(F("Увлажнитель"), 0, 2, isCurrentCursor(2), 1);
    printText(F("Вентиляция"), 0, 3, isCurrentCursor(3), 1);
  }
}

void IncubatorDisplay::testingCategory_Controls() {
  if (_isLeftDouble) goToMainScreen();
  if (_isLeftSingle) _changeCurrentScreen(1, 2);

  if (false == true) {
    // Отключаем инкубацию и делаем рендер
    _forceRenderer.expire();
  } else {
    _defaultCursor_Controls(3);

    if (_isRightSingle) {
      // if (cursorPosition == 0) сирена 5 секунд
      // if (cursorPosition == 1) обогреватель
      // if (cursorPosition == 2) увлажнитель
      // if (cursorPosition == 3) вентиляция
    }
  }

  _resetButtons();
}

void IncubatorDisplay::_printBooleanAsText(boolean status) {
  if (status) oled.print(F("вкл"));
  else oled.print(F("выкл"));
}
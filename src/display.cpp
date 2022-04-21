#include <Arduino.h>
#include <display.h>
#include <GyverOLED.h>  
#include <AsyncDelay.h>

IncubatorDisplay::IncubatorDisplay(IncubatorSettings* setSettings, IncubatorController* setController) {
  settings = setSettings;
  controller = setController;
}

void IncubatorDisplay::begin() {
  oled.init();
  _renderer.start(intervalOfLoop, AsyncDelay::MILLIS);
  _forceRenderer.start(intervalOfForceRender, AsyncDelay::MILLIS);
  
  unsigned int timeout = settings->secondary.displayTimeout * 1000;

  if (timeout < 10000) timeout = 10000;

  _timeout.start(timeout, AsyncDelay::MILLIS);
}

void IncubatorDisplay::loop() {
  if (_timeoutIsEnabled && _timeout.isExpired()) {
    if (status) status = false;
    if (currentMenu != 0) goToMainScreen();
  } else {
    if (!status) status = true;
  }

  if (updateByButtons()) {
    _timeout.restart();
    _forceRenderer.expire();
  }

  oled.setPower(status);

  if ((_update && _renderer.isExpired())
      || _forceRenderer.isExpired())
  {
    render();
  }
}

void IncubatorDisplay::render() {
  boolean menuButtonsHandle = updateByButtons();
  oled.clear();
  oled.home();

  if (!status) menuButtonsHandle = false;

  switch (currentMenu) {
    case 1:
      if (menuButtonsHandle) selectCategoryScreen_Controls();
      else selectCategoryScreen();
      break;

    case 2:
      if (menuButtonsHandle) incubationCategory_Controls();
      else incubationCategory();
      break;

    case 3:
      if (menuButtonsHandle) settingsCategory_Controls();
      else settingsCategory();
      break;

    case 4:
      if (menuButtonsHandle) testingCategory_Controls();
      else testingCategory();
      break;  

    default:
      if (menuButtonsHandle) mainScreen_Controls();
      else mainScreen();
      break;
  }

  _update = false;
  _renderer.restart();
  _forceRenderer.restart();

  if (menuButtonsHandle) render();
}

void IncubatorDisplay::printText(String text, int x, int y, boolean invertText = false, int scale = 1) {
  oled.invertText(invertText);
  oled.setScale(scale);
  oled.setCursor(x, y * scale);
  oled.print(text);
}

boolean IncubatorDisplay::isCurrentCursor(int number) {
  return (number == cursorPosition);
}

void IncubatorDisplay::disableTimeout() {
  _timeoutIsEnabled = false;
}

boolean IncubatorDisplay::updateByButtons() {
  if (_isLeftSingle || _isLeftDouble || _isLeftHold
      || _isRightSingle  || _isRightDouble || _isRightHold
      || _isUpSingle || _isUpDouble || _isUpHold
      || _isDownSingle || _isDownDouble || _isUpHold)
  {
    return true;
  }

  return false;
}

void IncubatorDisplay::buttonSingleEvents(boolean left, boolean right, boolean up, boolean down) {
  _isLeftSingle = left;
  _isRightSingle = right;
  _isUpSingle = up;
  _isDownSingle = down;

  if (updateByButtons()) _forceRenderer.expire();
}

void IncubatorDisplay::buttonDoubleEvents(boolean left, boolean right, boolean up, boolean down) {
  _isLeftDouble = left;
  _isRightDouble = right;
  _isUpDouble = up;
  _isDownDouble = down;

  if (updateByButtons()) _forceRenderer.expire();
}

void IncubatorDisplay::buttonHoldEvents(boolean left, boolean right, boolean up, boolean down) {
  _isLeftHold = left;
  _isRightHold = right;
  _isUpHold = up;
  _isDownHold = down;

  if (updateByButtons()) _forceRenderer.expire();
}

void IncubatorDisplay::_resetButtons() {
  _isLeftSingle = false;
  _isLeftDouble = false;
  _isLeftHold = false;

  _isRightSingle = false;
  _isRightDouble = false;
  _isRightHold = false;

  _isUpSingle = false;
  _isUpDouble = false;
  _isUpHold = false;

  _isDownSingle = false;
  _isDownDouble = false;
  _isDownHold = false;
}
#include <led.h>
#include <WS2812.h>

WS2812 incubatorLed(1);

LED::LED() {}

void LED::begin() {
  incubatorLed.setOutput(PIN_LED);
}

void LED::loop() {
  if (_isAnyBlink()) _blinkLoop();
  
	incubatorLed.set_crgb_at(0, color);
  incubatorLed.sync();
}

void LED::blink(String c, int ms, int brightness = 255) {
  if (c == "r" && !_redBlink.isOn) {
    _redBlink.isOn = true;
    _redBlink.brightness = brightness;
    _redBlink.endMillis = millis() + ms;
    _redBlink.valueBefore = color.r;
  }
  if (c == "g" && !_greenBlink.isOn) {
    _greenBlink.isOn = true;
    _greenBlink.brightness = brightness;
    _greenBlink.endMillis = millis() + ms;
    _greenBlink.valueBefore = color.g;
  }
  if (c == "b" && !_blueBlink.isOn) {
    _blueBlink.isOn = true;
    _blueBlink.brightness = brightness;
    _blueBlink.endMillis = millis() + ms;
    _blueBlink.valueBefore = color.b;
  }
}

boolean LED::_isAnyBlink() {
  if (_redBlink.isOn
      || _greenBlink.isOn
      || _blueBlink.isOn)
  {
    return true;
  }

  return false;
}

void LED::_blinkLoop() {
  if (_redBlink.isOn) {
    if (millis() > _redBlink.endMillis) {
      _redBlink.isOn = false;
      color.r = _redBlink.valueBefore;
    } else {
      color.r = _redBlink.brightness;
    }
  }

  if (_greenBlink.isOn) {
    if (millis() > _greenBlink.endMillis) {
      _greenBlink.isOn = false;
      color.g = _greenBlink.valueBefore;
    } else {
      color.g = _greenBlink.brightness;
    }
  }

  if (_blueBlink.isOn) {
    if (millis() > _blueBlink.endMillis) {
      _blueBlink.isOn = false;
      color.b = _blueBlink.valueBefore;
    } else {
      color.b = _blueBlink.brightness;
    }
  }
}
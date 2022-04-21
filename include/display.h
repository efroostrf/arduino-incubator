#ifndef display_h
  #define display_h

  #include <Arduino.h>
  #include <GyverOLED.h>  
  #include <AsyncDelay.h>
  #include <settings.h>
  #include <controller.h>

  class IncubatorDisplay {
    public:
      IncubatorDisplay(IncubatorSettings* setSettings, IncubatorController* setController);
      GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled;

      boolean status = true;
      int currentMenu = 0; // Выбранное меню
      int cursorPosition = 0; // Позиция курсора в меню

      // Показатели с датчиков
      float temperature = 0.0;
      int humidity = 0;
      int waterPercent = 0;

      IncubatorSettings* settings;
      IncubatorController* controller;
  
      int intervalOfLoop = 1000;
      int intervalOfForceRender = 5000;
   
      void begin();
      void loop();
      void render();
      void changeTimeout(int newTimeout);
      void power(boolean newStatus);
      void disableTimeout();

      // Меню и функции
      void goToMainScreen();

      void mainScreen();
      void selectCategoryScreen();
      void mainScreen_Controls();
      void selectCategoryScreen_Controls();

      void incubationCategory();
      void settingsCategory();
      void testingCategory();
      void incubationCategory_Controls();
      void settingsCategory_Controls();
      void testingCategory_Controls();

      void printText(String text, int x, int y, boolean invertText, int scale);
      boolean isCurrentCursor(int number);

      // Хендлер для кнопок
      void buttonSingleEvents(boolean left, boolean right, boolean up, boolean down);
      void buttonDoubleEvents(boolean left, boolean right, boolean up, boolean down);
      void buttonHoldEvents(boolean left, boolean right, boolean up, boolean down);
      boolean updateByButtons();

    private:
      boolean _timeoutIsEnabled = true;
      AsyncDelay _renderer;
      AsyncDelay _forceRenderer;
      AsyncDelay _timeout;
      boolean _update = true;
      boolean _forceUpdate = false;

      boolean _isLeftSingle = false;
      boolean _isRightSingle = false;
      boolean _isUpSingle = false;
      boolean _isDownSingle = false;

      boolean _isLeftDouble = false;
      boolean _isRightDouble = false;
      boolean _isUpDouble = false;
      boolean _isDownDouble = false;

      boolean _isLeftHold = false;
      boolean _isRightHold = false;
      boolean _isUpHold = false;
      boolean _isDownHold = false;

      void _resetButtons();
      void _changeCurrentScreen(int newScreen, int cursorPosition);
      void _defaultCursor_Controls(int maxY, boolean holdingChange);
      void _printBooleanAsText(boolean status);
  };
#endif
#ifndef constants_h
  #define constants_h

  #include <Arduino.h>

  #define COM_BAUNDRATE           9600

  // Подключения на цифровых пинах
  #define PIN_LED                 8   // Светодиод
  #define PIN_RelayHeater         5   // Реле нагревательного элемента
  #define PIN_RelayHydration      11  // Реле для контроля влажности
  #define PIN_RelayVenting        12  // Реле для контроля проветривания
  #define PIN_ESP8266_STATE       4   // Включение и отключение ESP8266
  #define PIN_ESP8266_RX          2
  #define PIN_ESP8266_TX          3
  #define PIN_IsPowered           6   // Есть ли напряжение с блока питания
  #define PIN_Buzzer              13

  // Аналоговые пины
  #define PIN_Water               A0  // Датчик заполнености ёмкости с водой
  #define PIN_Light               A1  // Датчик света
  #define PIN_BatteryVoltage      A6  // Напряжение батареи
  #define PIN_Buttons             A2  // Кнопки управления 

  // Дисплей
  #define DISPLAY_INTERVAL        1000
  #define DISPLAY_FORCE_INTERVAL  5000
  #define DISPLAY_TIMEOUT         60000

  #define SENSORS_UPDATE_INTERVAL 50
  
  // EEPROM индексы для сохранения данных
  #define EEPROM_SAVE_MODE        0   // Выбранный режим работы
  #define EEPROM_SAVE_PAUSE       2   // Приостановлена ли работа (не уверен, что это нужно)
  #define EEPROM_SAVE_STARTTIME   4   // Время начала инкубации
  #define EEPROM_MANUAL_STRUCT    50  // Struct с данными о ручной инкубации
  #define EEPROM_SECONDARY_STRUCT 100 // Struct с второстепенными, но не менее важными настройками
  #define EEPROM_SAVE_VENTING     20  // Сохранение unix времени последнего вентилирования
#endif
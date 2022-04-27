#ifndef programs_h
  #define programs_h

  #include <Arduino.h>

  struct INCUBATOR_PROGRAM {
    float temperature; //Температура
    int humidity; //Влажность
    int ventingCount; //Количество вентиляций
    int ventingTime; //Длительность каждой вентиляции
    int rotateCount; //Количество переворотов
  };
    
  //Цыплята
  const INCUBATOR_PROGRAM chickensProgram[] PROGMEM = {
    //Темп, Влаж, Вент, Вент. Дл., Повороты
    { 37.8, 60,   0,    0,         6 }, //  1
    { 37.8, 60,   0,    0,         6 }, //  2
    { 37.8, 60,   0,    0,         6 }, //  3
    { 37.8, 60,   0,    0,         6 }, //  4
    { 37.8, 60,   0,    0,         6 }, //  5
    { 37.8, 55,   0,    0,         6 }, //  6
    { 37.8, 55,   0,    0,         6 }, //  7
    { 37.8, 55,   0,    0,         6 }, //  8
    { 37.8, 55,   0,    0,         6 }, //  9
    { 37.8, 50,   0,    0,         6 }, // 10
    { 37.8, 50,   0,    0,         6 }, // 11
    { 37.8, 50,   0,    0,         6 }, // 12
    { 37.8, 50,   0,    0,         6 }, // 13
    { 37.8, 50,   0,    0,         6 }, // 14
    { 37.8, 45,   2,    15,        6 }, // 15
    { 37.8, 45,   2,    15,        6 }, // 16
    { 37.8, 45,   2,    15,        6 }, // 17
    { 37.8, 45,   2,    15,        6 }, // 18
    { 37.5, 75,   0,    0,        0 }, // 19
    { 37.5, 75,   0,    0,        0 }, // 20
    { 37.5, 75,   0,    0,        0 }  // 21
  };

#endif
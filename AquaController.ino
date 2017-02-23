//Подключаем общие библиотеки
#include <SPI.h>
#include <Wire.h>

//Подключаем и настраиваем библиотеки для экрана
#include "ASOLED.h"
#include "icons.c"

//Подключаем и настраиваем библиотеки для часов
#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);


// Определяем глобальные переменные
bool lampstate = true; //состояние лампы
bool airstate = true; //состояние компрессора
byte alarms[8] = {//массив для хранения будильников лампы и компрессора
10, 00, //вкл лампы
22, 00, //выкл лампы
10, 00, //вкл компрессор
22, 00 //выкл компрессор
};

void setup()   {                
  Serial.begin(9600);
  
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);

  Serial.println("Init screen");
  LD.init();  //initialze OLED display
  delay(1000);
  LD.clearDisplay();
  
  //--------RTC SETUP ------------
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  
  if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");

        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue

        Rtc.SetDateTime(compiled);
    }
    
    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
}

void loop() {
  if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    RtcDateTime now = Rtc.GetDateTime();
    printTime(now);
    printDate(now);
    drawLampState(lampstate);
    lampstate = !lampstate;
    drawAirState(airstate);
    airstate = !airstate;
  delay(1000);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printTime(const RtcDateTime& dt){ //Выводим текущее время
    char datestring[6];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u"),
            dt.Hour(),
            dt.Minute() );
    LD.printString_12x16(datestring, 62, 0);
}

void printDate(const RtcDateTime& dt){ //Выводим текущую дату
    char datestring[11];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u"),
            dt.Month(),
            dt.Day(),
            dt.Year() );
    LD.printString_6x8(datestring, 0, 0);
}

void drawLampState(bool state){//Выводим состояние лампы
  char timestring[6];
  
  if(state){
    LD.drawBitmap(lamp_on, 0, 2);
  }
  else{
    LD.drawBitmap(lamp_off, 0, 2);
  }
  LD.printString_6x8("ON Time: ", 18, 2);
  snprintf_P(timestring, 
            countof(timestring),
            PSTR("%02u:%02u"),
            alarms[0],
            alarms[1] );
  LD.printString_6x8(timestring, 73, 2);
  LD.printString_6x8("OFF Time: ", 18, 3);
  snprintf_P(timestring, 
            countof(timestring),
            PSTR("%02u:%02u"),
            alarms[2],
            alarms[3] );
  LD.printString_6x8(timestring, 73, 3);
}

void drawAirState(bool state){
  char timestring[6];
  
  if(state){
    LD.drawBitmap(air_on, 0, 5);
  }
  else{
    LD.drawBitmap(air_off, 0, 5);
  }
  LD.printString_6x8("ON Time: ", 18, 5);
  snprintf_P(timestring, 
            countof(timestring),
            PSTR("%02u:%02u"),
            alarms[4],
            alarms[5] );
  LD.printString_6x8(timestring, 73, 5);
  LD.printString_6x8("OFF Time: ", 18, 6);
  snprintf_P(timestring, 
            countof(timestring),
            PSTR("%02u:%02u"),
            alarms[6],
            alarms[7] );
  LD.printString_6x8(timestring, 73, 6);
}

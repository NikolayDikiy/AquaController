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

void setup()   {                
  Serial.begin(9600);
  
  //Serial.print("compiled: ");
  //Serial.print(__DATE__);
  //Serial.print(" ");
  //Serial.println(__TIME__);

  Serial.println("Init screen");
  LD.init();  //initialze OLED display
  LD.clearDisplay();
  delay(1000);

  // Clear the buffer.
  LD.clearDisplay();
  
  /* Display test lines
  *
  * testdrawliner();
  * delay(10000);
  * display.clearDisplay();
  */
/*
  // miniature bitmap display
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display.display();
  delay(10000);
*/
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

bool lampstate = true;

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
  delay(1000);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printTime(const RtcDateTime& dt)
{
    char datestring[6];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u"),
            dt.Hour(),
            dt.Minute() );
    LD.printString_12x16(datestring, 60, 0);
}

void printDate(const RtcDateTime& dt)
{
    char datestring[11];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u"),
            dt.Month(),
            dt.Day(),
            dt.Year() );
    LD.printString_6x8(datestring, 0, 0);
}

void drawLampState(bool state){
  if(state){
    LD.drawBitmap(lamp_on, 10, 5);
    //LD.drawBitmap(pill, 10, 5);
  }
  else{
    //LD.drawBitmap(lamp_off, 10, 5);
    LD.drawBitmap(pill, 10, 5);
  }
}

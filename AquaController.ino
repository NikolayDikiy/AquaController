//Подключаем общие библиотеки
#include <SPI.h>
#include <Wire.h>

//Подключаем и настраиваем библиотеки для экрана
#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <OakOLED.h>

//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);
OakOLED display;

/*
//Проверка настроек в библиотеке для нашего экрана
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
*/

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

  //Serial.println("Init screen");
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.begin();  // initialize with the I2C addr 0x3C (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  
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
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(67,0);
    display.fillRect(66,0,127,14, BLACK);
    display.print(datestring);
    display.display();
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
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.fillRect(0,0,66,7, BLACK);
    display.print(datestring);
    display.display();
}

void drawLampState(bool state){
  
  display.drawRect(10, 50, 10, 10, WHITE);
  if(state){
    display.fillCircle(15, 40, 10, WHITE);
  }
  else{
    display.fillCircle(15, 40, 10, BLACK);
    display.drawCircle(15, 40, 10, WHITE);
  }
  display.display();
}

// For some Tests
void testdrawliner(){
  for (int16_t i=0; i<display.width(); i+=2) {
    display.drawPixel(i, 0, WHITE);
    delay(1);
  }
  for (int16_t i=0; i<display.width(); i+=2) {
    display.drawPixel(i, 15, WHITE);
    delay(1);
  }
  for (int16_t i=0; i<display.width(); i+=2) {
    display.drawPixel(i, 16, WHITE);
    delay(1);
  }
  for (int16_t i=0; i<display.width(); i+=2) {
    display.drawPixel(i, 63, WHITE);
    delay(1);
  }
  for (int16_t i=0; i<display.width(); i+=5) {
    display.drawLine(i, 0, i, display.height(), WHITE);
    delay(1);
  }
  display.display();
}


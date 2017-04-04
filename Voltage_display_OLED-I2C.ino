#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
  
#include <TimeLib.h>

// Voltage Divider definitions
#define DIVIDER_R1 2000
#define DIVIDER_R2 10000

static float voltageAbsMax = 5.0 * ((DIVIDER_R1 + DIVIDER_R2) / DIVIDER_R1);

#define VOLTAGEPIN 6

float inputVoltage=0.0;
float historicTopVoltage=6.0;
  
//#define 
int voltageHistory [48*6];

#define MAXYLINES 48

void setup()   {                
//  Serial.begin(9600);

  u8g2.begin();
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

}

int calculateDisplayVoltLevel(float displayVoltage) {
  int lvl = (int) MAXYLINES * displayVoltage / historicTopVoltage;
  return (lvl);
}


void loop() {
//  byte historyIndex = (hour()*4)+(minute()/15);  // production dataset index
  byte historyIndex = (second());  // test dataset index
  byte x = 0;
  
  historicTopVoltage = 6.0;

  voltageHistory[historyIndex] = analogRead(VOLTAGEPIN);
  inputVoltage=voltageHistory[historyIndex]*voltageAbsMax/1024;  

  char str[10];

  for (x=0; x<(24*4); x++)
  {
    if ((voltageHistory[x]*voltageAbsMax/1024)>historicTopVoltage)
    {
      historicTopVoltage=voltageHistory[x]*voltageAbsMax/1024;
    }
  }
  
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenR08_tf);
    (String(hour()) + ":" + String(minute())+ ":" + String(second())).toCharArray(str,10);
    u8g2.drawStr( 0, 0, str);
    (String(inputVoltage) + "V").toCharArray(str,10);
    u8g2.drawStr( 90, 0, str);
    u8g2.drawLine(30,10, 30, 63);
    (String(historicTopVoltage)).toCharArray(str,10);
    u8g2.drawStr(0,10, str);
    for (x=0; x<(24*4); x++)
    {
      u8g2.drawPixel (31+x, 63 - calculateDisplayVoltLevel(voltageHistory[x]));
    }
  } while ( u8g2.nextPage() );

  delay (500);
}



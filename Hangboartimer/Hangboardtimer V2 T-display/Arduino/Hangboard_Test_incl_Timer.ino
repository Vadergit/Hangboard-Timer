/**
  First demo for FT6206 Capactive Touch Screen on Wokwi. Enjoy!

  https://wokwi.com/arduino/projects/311598148845830720
*/

/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341
  captouch shield
  ----> http://www.adafruit.com/products/1947

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <TFT_eSPI.h>
#include <SPI.h>   // this is needed for display
#include <Wire.h>  // this is needed for FT6206
#include "pictures.h"

#define LOAD_FONT8

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library with default width and height
int brightness = 255;       // 0-255 8bit



// Size of the color selection boxes and the paintbrush size
#define TFTHEIGHT 170
#define TFTWITH 320

//Timer
int s, ms;
float sekunden;
unsigned long start, finished, elapsed;

unsigned long millis();
unsigned long previusMillis2 = 0;
unsigned long previusMillis3 = 0;
int state = 0;
unsigned long previousMillis = 0;
unsigned long guestT, darioT, anitaT;
unsigned long guestTd, darioTd, anitaTd;
unsigned long usertime;
// Rotary Encoder Inputs
#define CLK 1
#define DT 2

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";

int user = 0;


void setup(void) {

  pinMode(21, INPUT_PULLUP);

  pinMode(3, INPUT);
  pinMode(16, INPUT);
  pinMode(15, OUTPUT);  //battery enable
  digitalWrite(15, 1);  //battery enable
  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);
  ledcWrite(0, brightness);




  // Read the initial state of CLK
  lastStateCLK = digitalRead(3);

  // Call updateEncoder() when any high/low changed seen
  // on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(3, updateEncoder, CHANGE);
  attachInterrupt(16, updateEncoder, CHANGE);
  tft.begin();

  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);



  tft.drawBitmap(TFTWITH - 70, 0, boulderimages, 70, 54, TFT_WHITE);

  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Palm, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Palml, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Mittel, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Mittell, 36, 48, TFT_WHITE);
  delay(500);
  
}

void loop() {
  updateEncoder();


  if (digitalRead(21) == LOW) {
    unsigned long currentMillis = millis();
    previusMillis2 = (currentMillis - previousMillis);  ///

    finished = previusMillis2;
  }

  if (digitalRead(21) == HIGH) {

    previousMillis = millis();
    if (previusMillis3 != previusMillis2) {
      usertime += finished;
      previusMillis3 = previusMillis2;
    }
  }

  unsigned long over;
  elapsed = finished - start;
  over = elapsed % 3600000;
  over = over % 60000;
  s = int(over / 1000);
  ms = over % 1000;
  sekunden = (((s * 1000) + ms) / 1000);

  // Drawing stp watch
  tft.setTextSize(4);

  tft.setCursor(0, TFTHEIGHT - 30);
  if (elapsed >= 60000) {  ////////Adding min/////////////////
    tft.print(elapsed / 1000 / 60);
    tft.print("min ");
    tft.print(sekunden, 2);
    tft.print("s");
  }

  if (elapsed <= 60000) {  /////////removing ms/////////////////
    tft.print(s);
    tft.print("s ");
    tft.print(ms);
    tft.print("ms ");
  }



  drawPicture();
  //Drawing user
  tft.setTextSize(2);
  tft.setCursor(0, 105);
  tft.println("user:");
  userSelect();
}

void drawPicture() {

  tft.setTextSize(3);
  tft.setCursor(0, 0);
  tft.println("Hangboardtimer");


  /// update alle the collected times

  unsigned long over;
  elapsed = guestT - start;
  over = elapsed % 3600000;
  over = over % 60000;
  s = int(over / 1000);
  ms = over % 1000;


  tft.setTextSize(2);
  tft.setCursor(0, 33);
  tft.print("Guest: ");
  tft.print(guestT / 60000);
  tft.print("min ");
  tft.print(s);
  tft.println("s");

  elapsed = anitaT - start;
  over = elapsed % 3600000;
  over = over % 60000;
  s = int(over / 1000);
  ms = over % 1000;

  tft.print("Anita: ");
  tft.print(anitaT / 60000);
  tft.print("min ");
  tft.print(s);
  tft.println("s");

  elapsed = darioT - start;
  over = elapsed % 3600000;
  over = over % 60000;
  s = int(over / 1000);
  ms = over % 1000;
  tft.print("Dario: ");
  tft.print(darioT / 60000);
  tft.print("min ");
  tft.print(s);
  tft.println("s");




  tft.drawBitmap(TFTWITH - 70, 0, boulderimages, 70, 54, TFT_WHITE);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Palm, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Palml, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Mittel, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Ring, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_zeige, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_klein, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Mittell, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Ringl, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_zeigel, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_kleinl, 36, 48, TFT_WHITE);
}

void updateEncoder() {
  // Read the current state of CLK
  currentStateCLK = digitalRead(16);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(3) != currentStateCLK) {


      counter--;
      finished = 0;
      start = 0;
       
      delay(500);
      tft.setTextSize(4);
      tft.setCursor(0, TFTHEIGHT - 30);
       tft.print("         ");
      if (counter <= -1) {
        counter = 3;
        delay(500);
      }
      currentDir = "CCW";
    } else {
      // Encoder is rotating CW so increment
      counter++;
      finished = 0;
      start = 0;
      delay(500);
      tft.setTextSize(4);
      tft.setCursor(0, TFTHEIGHT - 30);
       tft.print("         ");
      if (counter >= 3) {
        counter = 0;
        delay(500);
      }
      currentDir = "CW";
    }
  }

  // Remember last CLK state
  lastStateCLK = currentStateCLK;
}


void userSelect() {
  user = counter;
  switch (user) {
    case 0:
      tft.setCursor(60, 105);
      tft.setTextSize(3);
      tft.print("Guest");
      guestT += usertime;
      usertime = 0;

      break;
    case 1:  // your hand is close to the sensor
      tft.setCursor(60, 105);
      tft.setTextSize(3);
      tft.print("Anita");
      anitaT += usertime;
      usertime = 0;
      break;
    case 2:
      tft.setCursor(60, 105);
      tft.setTextSize(3);
      tft.print("Dario");
      darioT += usertime;
      usertime = 0;
  }
}

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
#include "AiEsp32RotaryEncoder.h"
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
int counter =3;
#define ROTARY_ENCODER_A_PIN 16
#define ROTARY_ENCODER_B_PIN 3
#define ROTARY_ENCODER_BUTTON_PIN 21
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_STEPS 1
//#define ROTARY_ENCODER_STEPS 2
#define ROTARY_ENCODER_STEPS 4

//instead of changing here, rather change numbers above
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);


 int user = 0;

void rotary_onButtonClick()
{
	static unsigned long lastTimePressed = 0;
	//ignore multiple press in that time milliseconds
	if (millis() - lastTimePressed < 500)
	{
		return;
	}
	lastTimePressed = millis();
//	Serial.print("button pressed ");
//	Serial.print(millis());
//	Serial.println(" milliseconds after restart");
}
void rotary_loop()
{
	//dont print anything unless value changed
	if (rotaryEncoder.encoderChanged())
	{
	//	Serial.print("Value: ");
	//	Serial.println(rotaryEncoder.readEncoder());
  user= rotaryEncoder.readEncoder();
  finished=0;
  tft.setTextSize(4);

  tft.setCursor(0, TFTHEIGHT - 30);
    tft.print("         ");

	}
	if (rotaryEncoder.isEncoderButtonClicked())
	{
		rotary_onButtonClick();
	}
}
void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}


void setup(void) {

  pinMode(21, INPUT_PULLUP);

  pinMode(3, INPUT);
  pinMode(16, INPUT);
  pinMode(15, OUTPUT);  //battery enable
  digitalWrite(15, 1);  //battery enable
  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);
  ledcWrite(0, brightness);




//we must initialize rotary encoder
	rotaryEncoder.begin();
	rotaryEncoder.setup(readEncoderISR);
	//set boundaries and if values should cycle or not
	//in this example we will set possible values between 0 and 1000;
	bool circleValues = false;
	rotaryEncoder.setBoundaries(0, counter, true); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

	/*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
	//rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
	rotaryEncoder.setAcceleration(0); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration





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
 rotary_loop();


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





void userSelect() {
 // user = counter;
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

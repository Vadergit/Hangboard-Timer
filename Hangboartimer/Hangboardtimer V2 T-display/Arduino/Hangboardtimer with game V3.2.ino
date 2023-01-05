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


/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 https://randomnerdtutorials.com/esp32-esp8266-publish-sensor-readings-to-google-sheets/
 */


/*
AI snipplets

unsigned long times[3] = {guestT, darioT, anitaT};
unsigned long timeD[3] = {guestTd, darioTd, anitaTd};
char* names[3] = {"Guest", "Dario", "Anita"};

for (int i = 0; i < 3; i++) {
  if (times[i] > usertime) {
    usertime = times[i];
    user = i;
  }
  if (timeD[i] > usertime) {
    usertime = timeD[i];
    user = i;
  }
}

//serial.println("Best time: " + String(names[user]) + " Time: " + String(usertime));



*/


#include <TFT_eSPI.h>
#include "pictures.h"
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <EEPROM.h>
#include <HX711_ADC.h>

#include <WiFi.h>

#include <NTPClient.h>

//NTC Time

const long utcOffsetInSeconds = 7200;


char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
int dayofmonth = 0;

// Replace with your unique IFTTT URL resource
const char* resource = "/trigger/hangboard_event/with/key/ndIFb9X0T8pyRO4Wgkbt_O9vkjrAP9XpdnCgAX2V9Qe";

// How your resource variable should look like, but with your own API KEY (that API KEY below is just an example):
//const char* resource = "/trigger/bme280_readings/with/key/nAZjOphL3d-ZO4N3k64-1A7gTlNSrxMJdmqy3";

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";




int mod = 0;

//pins:
const int HX711_doutL = 12;  //mcu > HX711 dout pin
const int HX711_sckL = 13;   //mcu > HX711 sck pin

//pins:
const int HX711_doutR = 10;  //mcu > HX711 dout pin
const int HX711_sckR = 11;   //mcu > HX711 sck pin
// HX711 circuit wiring

//const int sw = 2
//const int data= 16
//const int clk=3

int difRL = 69;
int minpos = -81;
int maxpos = 219;
float forceR = 0;
float forceL = 0;
float load = 0;
int difRLConstrain;

float maxload = 0;
int Start = 0;

HX711_ADC LoadCellL(HX711_doutL, HX711_sckL);

HX711_ADC LoadCellR(HX711_doutR, HX711_sckR);
unsigned long t = 0;

#define BUTTON_RIGHT 14
#define BUTTON_SW 2

// TTGO: 240x135
#define TFTW 320          // screen width
#define TFTH 170          // screen height
#define TFTW2 (TFTW / 2)  // half screen width
#define TFTH2 (TFTH / 2)  // half screen height

// game constant
#define SPEED 1
#define GRAVITY 0        //#define GRAVITY 9.8
#define JUMP_FORCE 0     //#define JUMP_FORCE 2.15
#define SKIP_TICKS 20.0  // 1000 / 50fps
#define MAX_FRAMESKIP 5

// bird size
#define BIRDW 16  // bird width
#define BIRDH 16  // bird height
#define BIRDW2 8  // half width
#define BIRDH2 8  // half height
// pipe size
#define PIPEW 24      // pipe width
#define GAPHEIGHT 42  // pipe gap height
// floor size
#define FLOORH 30  // floor height (from bottom of the screen)
// grass size
#define GRASSH 4  // grass height (inside floor, starts at floor y)

#define COLOR565(r, g, b) ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

//auto tft = TFT_eSPI();  // Invoke custom library
uint maxScore = 0;

// background
#define BCKGRDCOL COLOR565(138, 235, 244)
// bird
#define BIRDCOL COLOR565(255, 254, 174)
// pipe
#define PIPECOL COLOR565(99, 255, 78)
// pipe highlight
#define PIPEHIGHCOL COLOR565(250, 255, 250)
// pipe seam
#define PIPESEAMCOL COLOR565(0, 0, 0)
// floor
#define FLOORCOL COLOR565(246, 240, 163)
// grass (col2 is the stripe color)
#define GRASSCOL COLOR565(141, 225, 87)
#define GRASSCOL2 COLOR565(156, 239, 88)

// bird sprite
// bird sprite colors (Cx name for values to keep the array readable)
#define C0 BCKGRDCOL
#define C1 COLOR565(195, 165, 75)
#define C2 BIRDCOL
#define C3 TFT_WHITE
#define C4 TFT_RED
#define C5 COLOR565(251, 216, 114)

static const unsigned int birdcol[] = {
  C0, C0, C1, C1, C1, C1, C1, C0, C0, C0, C1, C1, C1, C1, C1, C0,
  C0, C1, C2, C2, C2, C1, C3, C1, C0, C1, C2, C2, C2, C1, C3, C1,
  C0, C2, C2, C2, C2, C1, C3, C1, C0, C2, C2, C2, C2, C1, C3, C1,
  C1, C1, C1, C2, C2, C3, C1, C1, C1, C1, C1, C2, C2, C3, C1, C1,
  C1, C2, C2, C2, C2, C2, C4, C4, C1, C2, C2, C2, C2, C2, C4, C4,
  C1, C2, C2, C2, C1, C5, C4, C0, C1, C2, C2, C2, C1, C5, C4, C0,
  C0, C1, C2, C1, C5, C5, C5, C0, C0, C1, C2, C1, C5, C5, C5, C0,
  C0, C0, C1, C5, C5, C5, C0, C0, C0, C0, C1, C5, C5, C5, C0, C0
};

// bird structure
static struct BIRD {
  long x, y, old_y;
  long col;
  float vel_y;
} bird;

// pipe structure
static struct PIPES {
  long x, gap_y;
  long col;
} pipes;

// score
int score;
// temporary x and y var
static short tmpx, tmpy;

// ---------------
// draw pixel
// ---------------
// faster drawPixel method by inlining calls and using setAddrWindow and pushColor using macro to force inlining
#define drawPixel(a, b, c) \
  tft.setAddrWindow(a, b, a, b); \
  tft.pushColor(c)


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
#define CLK 3
#define DT 16

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";

int user = 0;




void setup(void) {

  //Serial.begin(115200);


  // put your setup code here, to run once:

  pinMode(15, OUTPUT);  //battery enable
  digitalWrite(15, 1);  //battery enable
  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);
  ledcWrite(0, brightness);

  pinMode(2, INPUT_PULLUP);






  LoadCellL.begin();
  LoadCellR.begin();

  unsigned long stabilizingtime = 0;  // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;               //set this to false if you don't want tare to be performed in the next step
  byte loadcellL_rdy = 0;
  byte loadcellR_rdy = 0;
  while ((loadcellL_rdy + loadcellR_rdy) < 2) {  //run startup, stabilization and tare, both modules simultaniously
    if (!loadcellL_rdy) loadcellL_rdy = LoadCellL.startMultiple(stabilizingtime, _tare);
    if (!loadcellR_rdy) loadcellR_rdy = LoadCellR.startMultiple(stabilizingtime, _tare);
  }
  if (LoadCellL.getTareTimeoutFlag()) {
    //serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
  }
  if (LoadCellR.getTareTimeoutFlag()) {
    //serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
  }
  LoadCellL.setCalFactor(20230);  // user set calibration value (float)
  LoadCellR.setCalFactor(20230);  // user set calibration value (float)
  //serial.println("Startup is complete");







  tft.begin();
  tft.setRotation(3);
  tft.setSwapBytes(true);

  // reset max score
  EEPROM.writeUInt(0, 0);

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(0, 0);
  tft.println("Hangboardtimer");
  tft.setTextSize(2);
  tft.setCursor(0, 50);
  tft.println("Please config your WiFi...");
  tft.setCursor(0, 70);
  tft.println("Connect to: WiFi Hangboard");
  tft.println("password  : password");

  /*
  tft.drawBitmap(TFTWITH - 70, 0, boulderimages, 70, 54, TFT_WHITE);

  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Palm, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Palml, 36, 48, TFT_WHITE);
  delay(500);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Mittel, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Mittell, 36, 48, TFT_WHITE);
  delay(1500);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Mittel, 36, 48, TFT_BLACK);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Mittell, 36, 48, TFT_BLACK);
  delay(1500);
  tft.drawBitmap(TFTWITH - 36, TFTHEIGHT - 48, epd_bitmap_Mittel, 36, 48, TFT_WHITE);
  tft.drawBitmap(TFTWITH - (2 * 36) - 4, TFTHEIGHT - 48, epd_bitmap_Mittell, 36, 48, TFT_WHITE);
  delay(1500);
*/

  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // it is a good practice to make sure your code sets wifi mode how you want it.
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  // wm.setClass("invert"); // dark theme
  // wm.setScanDispPerc(true); // display percentages instead of graphs for RSSI


  // wm.setSTAStaticIPConfig()
  wm.setHostname("hangboard");
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("WiFi Hangboard", "password");  // password protected ap


  if (!res) {
    //serial.println("Failed to connect");

    // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    //serial.println("connected...yeey :)");
  }

  tft.fillScreen(TFT_BLACK);
}

void loop() {

  //Serial.println(mod);
  updateLoadcell();

  hang();


  if (mod == 1) {
    game_start();
    game_loop();
    game_over();

    if (digitalRead(2) == LOW) {
      mod = 0;
      delay(500);
    }
  }

  if (digitalRead(2) == LOW) {
    mod = 1;
    delay(500);
  }
}

/*
    game_start();
    game_loop();
    game_over();
    */

void hang() {
  if (mod == 0) {
    //updateEncoder();

    if (digitalRead(Start) == HIGH) {
      unsigned long currentMillis = millis();
      previusMillis2 = (currentMillis - previousMillis);  ///

      finished = previusMillis2;
    }

    if (digitalRead(Start) == LOW) {

      previousMillis = millis();
      if (previusMillis3 != previusMillis2) {
        usertime += finished;
        makeIFTTTRequest();
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
}

void drawPicture() {

  tft.setTextSize(3);
  tft.setCursor(0, 0);
  tft.println("Hangboardtimer");
  tft.setCursor(180, 90);
  tft.print(load, 1);
  // tft.print(maxload, 1);

  tft.println("kg ");



  /// update alle the collected times




  unsigned long over;

  elapsed = guestT - start;
  over = elapsed % 3600000;
  over = over % 60000;
  s = int(over / 1000);
  ms = over % 1000;


  tft.setTextSize(2);
  tft.setCursor(0, 30);
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
  //user = counter;


  if ((maxload >= 65) && (maxload <= 75)) {
    user = 1;
  }

  if ((maxload >= 76) && (maxload <= 90)) {
    user = 2;
  }


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


// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  //serial.print("Connecting to ");
  //serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    //serial.print(".");
  }
  //serial.println();
  if (!!!client.connected()) {
    //serial.println("Failed to connect...");
  }

  //serial.print("Request resource: ");
  //serial.println(resource);

  // Webhook Vairables
  String jsonObject = String("{\"value1\":\"") + ("0") + "\",\"value2\":\"" + ("0")
                      + "\",\"value3\":\"" + ("0") + "\"}";

  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10;  // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    //serial.println("No response...");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  //serial.println("\nclosing connection");
  client.stop();

  //////////////////////////////////////////////////////////////////////////////////////retrys///////////////////////////


  /*
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    //serial.print(".");
  }
  //serial.println();
  if (!!!client.connected()) {
    //serial.println("Failed to connect...");
  }

  //serial.print("Request resource: ");
  //serial.println(resource);

  // Webhook Vairables
  String jsonObject = String("{\"value1\":\"") + (guestT) + "\",\"value2\":\"" + (anitaT)
                      + "\",\"value3\":\"" + (darioT) + "\"}";

  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server);
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);

  int timeout = 5 * 10;  // 5 seconds
  while (!!!client.available() && (timeout-- > 0)) {
    delay(100);
  }
  if (!!!client.available()) {
    //serial.println("No response...");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  //serial.println("\nclosing connection");
  client.stop();
}

void daycheck() {
  timeClient.update();

  if (timeClient.getDay() != dayofmonth) {
    dayofmonth = timeClient.getDay();
    //serial.println("day has changed");

    guestTd = 0;
    anitaTd = 0;
    darioTd = 0;
  }
  */
}




















void game_init() {
  // clear screen
  tft.fillScreen(BCKGRDCOL);
  // reset score
  score = 0;
  // init bird
  bird.x = 144;
  bird.y = bird.old_y = TFTH2 - BIRDH;
  bird.vel_y = -JUMP_FORCE;
  tmpx = tmpy = 0;
  // generate new random seed for the pipe gape
  randomSeed(analogRead(0));
  // init pipe
  pipes.x = 0;
  pipes.gap_y = random(20, TFTH - 60);
}

void game_start() {

  tft.fillScreen(TFT_BLACK);
  tft.fillRect(10, TFTH2 - 20, TFTW - 20, 1, TFT_WHITE);
  tft.fillRect(10, TFTH2 + 32, TFTW - 20, 1, TFT_WHITE);
  tft.setTextColor(TFT_WHITE);

  tft.setTextSize(3);
  // half width - num char * char width in pixels
  tft.setCursor(TFTW2 - (13 * 9), TFTH2 - 68);
  tft.println("HANGBOARDGAME");


  tft.setTextSize(3);
  // half width - num char * char width in pixels
  tft.setCursor(TFTW2 - (8 * 9), TFTH2 - 16);
  tft.println("BACKPACK");
  tft.setTextSize(3);
  tft.setCursor(TFTW2 - (11 * 9), TFTH2 + 8);
  tft.println("-MEOW-MEOW-");

  tft.setTextSize(2);
  tft.setCursor(TFTW2 - (18 * 6), TFTH2 + 58);
  tft.println("WEIGHTSHIFT or DIE");

  // wait for push button




  while (digitalRead(Start) == LOW)  //////////////////////////////////////////////////////
                                     // while (digitalRead(BUTTON_RIGHT) == HIGH)
    updateLoadcell();


  yield();

  // init game settings
  game_init();
}

void game_loop() {


  // ===============
  // prepare game variables
  // draw floor
  // ===============
  // instead of calculating the distance of the floor from the screen height each time store it in a variable
  const unsigned char GAMEH = TFTH - FLOORH;
  // draw the floor once, we will not overwrite on this area in-game
  // black line
  tft.drawFastHLine(0, GAMEH, TFTW, TFT_BLACK);
  // grass and stripe
  tft.fillRect(0, GAMEH + 1, TFTW2, GRASSH, GRASSCOL);
  tft.fillRect(TFTW2, GAMEH + 1, TFTW2, GRASSH, GRASSCOL2);
  // black line
  tft.drawFastHLine(0, GAMEH + GRASSH, TFTW, TFT_BLACK);
  // mud
  tft.fillRect(0, GAMEH + GRASSH + 1, TFTW, FLOORH - GRASSH, FLOORCOL);
  // grass x position (for stripe animation)
  long grassx = TFTW;
  // game loop time variables
  double delta, old_time, next_game_tick, current_time;
  next_game_tick = current_time = millis();
  // passed pipe flag to count score
  bool passed_pipe = false;
  // temp var for setAddrWindow
  unsigned char px;

  while (true) {
    yield();

    int loops = 0;
    while (millis() > next_game_tick && loops < MAX_FRAMESKIP) {
      // ===============
      // input
      // ===============

      updateLoadcell();





      // ===============
      // update
      // ===============
      // calculate delta time
      // ---------------
      old_time = current_time;
      current_time = millis();
      delta = (current_time - old_time) / 1000;





      // bird
      // ---------------
      bird.vel_y += GRAVITY * delta;
      bird.y += bird.vel_y;

      // pipe
      // ---------------
      pipes.x -= SPEED;
      // if pipe reached edge of the screen reset its position and gap
      if (pipes.x < -PIPEW) {
        pipes.x = TFTW;
        pipes.gap_y = random(10, GAMEH - (10 + GAPHEIGHT));
      }

      // ---------------
      next_game_tick += SKIP_TICKS;
      loops++;
    }

    // ===============
    // draw
    // ===============
    // pipe
    // ---------------
    // we save cycles if we avoid drawing the pipe when outside the screen
    if (pipes.x >= 0 && pipes.x < TFTW) {
      // pipe color
      tft.drawFastVLine(pipes.x + 3, 0, pipes.gap_y, PIPECOL);
      tft.drawFastVLine(pipes.x + 3, pipes.gap_y + GAPHEIGHT + 1, GAMEH - (pipes.gap_y + GAPHEIGHT + 1), PIPECOL);
      // highlight
      tft.drawFastVLine(pipes.x, 0, pipes.gap_y, PIPEHIGHCOL);
      tft.drawFastVLine(pipes.x, pipes.gap_y + GAPHEIGHT + 1, GAMEH - (pipes.gap_y + GAPHEIGHT + 1), PIPEHIGHCOL);
      // bottom and top border of pipe
      drawPixel(pipes.x, pipes.gap_y, PIPESEAMCOL);
      drawPixel(pipes.x, pipes.gap_y + GAPHEIGHT, PIPESEAMCOL);
      // pipe seam
      drawPixel(pipes.x, pipes.gap_y - 6, PIPESEAMCOL);
      drawPixel(pipes.x, pipes.gap_y + GAPHEIGHT + 6, PIPESEAMCOL);
      drawPixel(pipes.x + 3, pipes.gap_y - 6, PIPESEAMCOL);
      drawPixel(pipes.x + 3, pipes.gap_y + GAPHEIGHT + 6, PIPESEAMCOL);
    }
    // erase behind pipe
    if (pipes.x <= TFTW)
      tft.drawFastVLine(pipes.x + PIPEW, 0, GAMEH, BCKGRDCOL);

    // bird
    // ---------------
    tmpx = BIRDW - 1;
    do {
      px = bird.x + tmpx + BIRDW;
      // clear bird at previous position stored in old_y
      // we can't just erase the pixels before and after current position
      // because of the non-linear bird movement (it would leave 'dirty' pixels)
      tmpy = BIRDH - 1;
      do {
        drawPixel(px, bird.old_y + tmpy, BCKGRDCOL);
      } while (tmpy--);

      // draw bird sprite at new position
      tmpy = BIRDH - 1;
      do {
        drawPixel(px, bird.y + tmpy, birdcol[tmpx + (tmpy * BIRDW)]);
      } while (tmpy--);

    } while (tmpx--);
    // save position to erase bird on next draw
    bird.old_y = bird.y;

    // grass stripes
    // ---------------
    grassx -= SPEED;
    if (grassx < 0)
      grassx = TFTW;

    tft.drawFastVLine(grassx % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL);
    tft.drawFastVLine((grassx + 64) % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL2);

    // ===============
    // collision
    // ===============
    // if the bird hit the ground game over
    if (bird.y > GAMEH - BIRDH)
      break;

    // checking for bird collision with pipe
    if (bird.x + BIRDW >= pipes.x - BIRDW2 && bird.x <= pipes.x + PIPEW - BIRDW) {
      // bird entered a pipe, check for collision
      if (bird.y < pipes.gap_y || bird.y + BIRDH > pipes.gap_y + GAPHEIGHT)
        break;
      else
        passed_pipe = true;
    }
    // if bird has passed the pipe increase score
    else if (bird.x > pipes.x + PIPEW - BIRDW && passed_pipe) {
      passed_pipe = false;
      // erase score with background color
      tft.setTextColor(BCKGRDCOL);
      tft.setCursor(TFTW2, 4);
      tft.print(score);
      // set text color back to white for new score
      tft.setTextColor(TFT_WHITE);
      // increase score since we successfully passed a pipe
      score++;
    }

    // update score
    // ---------------
    tft.setCursor(TFTW2, 4);
    tft.print(score);
  }

  // add a small delay to show how the player lost
  delay(1200);
}

// ---------------
// game over
// ---------------
void game_over() {
  tft.fillScreen(TFT_BLACK);
  maxScore = EEPROM.readUInt(0);

  if (score > maxScore) {
    EEPROM.writeUInt(0, score);
    maxScore = score;
    tft.setTextColor(TFT_RED);
    tft.setTextSize(2);
    tft.setCursor(TFTW2 - (13 * 6), TFTH2 - 26);
    tft.println("NEW HIGHSCORE");
  }

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  // half width - num char * char width in pixels
  tft.setCursor(TFTW2 - (9 * 9), TFTH2 - 6);
  tft.println("GAME OVER");
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("score: ");
  tft.print(score);
  tft.setCursor(TFTW2 - (12 * 6), TFTH2 + 18);
  tft.println("press button");
  tft.setCursor(10, 28);
  tft.print("Max Score:");
  tft.print(maxScore);

  delay(1000);
  // wait for push button

  while (digitalRead(Start) == LOW) {  //////////////////////////////////////////////////////
                                       // while (digitalRead(BUTTON_RIGHT) == HIGH)
    updateLoadcell();
    yield();
  }
}



void updateLoadcell() {

  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;  //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCellL.update()) newDataReady = true;
  LoadCellR.update();

  //get smoothed value from data set
  if ((newDataReady)) {
    if (millis() > t + serialPrintInterval) {
      float a = LoadCellL.getData();
      float b = LoadCellR.getData();
      //  //serial.print("Load_cell 1 output val: ");
      //  //serial.print(a);
      //  //serial.print("    Load_cell 2 output val: ");
      //  //serial.println(b);
      newDataReady = 0;
      t = millis();

      forceL = a;
      forceR = b;
      load = forceL + forceR;
      difRL = (map(forceR, 0, load, maxpos, minpos));
      difRLConstrain = constrain(difRL, 0, 124);

      bird.y = difRLConstrain;
      if (load > maxload) {  // determine minimum value
        maxload = load;
      }
      if (load > 40) {
        Start = 0;

      } else {
        Start = 1;
        maxload = 0;
        bird.y = TFTH2 - BIRDH;
      }
      ////serial.print(" Diff val: ");
      ////serial.println(difRL);
    }
  }
  /*
  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCellL.tareNoDelay();
      LoadCellR.tareNoDelay();
    }
  }

  //check if last tare operation is complete
  if (LoadCellL.getTareStatus() == true) {
    //serial.println("Tare load cell 1 complete");
  }
  if (LoadCellR.getTareStatus() == true) {
    //serial.println("Tare load cell 2 complete");
  }
  */
}

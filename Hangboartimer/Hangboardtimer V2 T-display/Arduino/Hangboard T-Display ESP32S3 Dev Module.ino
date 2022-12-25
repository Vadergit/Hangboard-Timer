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

Serial.println("Best time: " + String(names[user]) + " Time: " + String(usertime));



*/


#include <TFT_eSPI.h>
#include "pictures.h"
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager


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

  Serial.begin(115200);

  // put your setup code here, to run once:

  pinMode(15, OUTPUT);  //battery enable
  digitalWrite(15, 1);  //battery enable
  ledcSetup(0, 10000, 8);
  ledcAttachPin(38, 0);
  ledcWrite(0, brightness);


  pinMode(21, INPUT_PULLUP);

  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(0);

  // Call updateEncoder() when any high/low changed seen
  // on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(2, updateEncoder, CHANGE);
  attachInterrupt(3, updateEncoder, CHANGE);
  tft.begin();
  tft.setRotation(3);

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
    Serial.println("Failed to connect");

    // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }

  tft.fillScreen(TFT_BLACK);
}

void loop() {
  /*
  if (!wm.connected) {

    tft.fillScreen(TFT_BLACK);
  tft.println("Please config your WiFi...");
  tft.setCursor(0, 70);
  tft.println("Connect to: WiFi Hangboard");
  tft.println("password  : password");
Serial.print("lost Connection");
  }
  */
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

void updateEncoder() {
  // Read the current state of CLK
  currentStateCLK = digitalRead(3);

  // If last and current state of CLK are different, then pulse occurred
  // React to only 1 state change to avoid double count
  if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {

    // If the DT state is different than the CLK state then
    // the encoder is rotating CCW so decrement
    if (digitalRead(2) != currentStateCLK) {


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


// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  Serial.print("Connecting to ");
  Serial.print(server);

  WiFiClient client;
  int retries = 5;
  while (!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!!!client.connected()) {
    Serial.println("Failed to connect...");
  }

  Serial.print("Request resource: ");
  Serial.println(resource);

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
    Serial.println("No response...");
  }
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("\nclosing connection");
  client.stop();
}

void daycheck() {
  timeClient.update();

  if (timeClient.getDay() != dayofmonth) {
    dayofmonth = timeClient.getDay();
    Serial.println("day has changed");

    guestTd = 0;
    anitaTd = 0;
    darioTd = 0;
  }
}

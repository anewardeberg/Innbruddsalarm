
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Keypad.h>
#include <SPI.h>

#include <Wire.h>
#include <RtcDS3231.h>

#define PINK 0xF81F
#define BLACK 0x0000
#define GREEN 0x07E0
#define WHITE 0xFFFF
#define RED 0xF800

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
#define TFT_CS         14
#define TFT_RST        15
#define TFT_DC         32

#elif defined(ESP8266)
#define TFT_CS         4
#define TFT_RST        16
#define TFT_DC         5

#else
// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS        10
#define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         8
#endif

#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


#define Solenoid 11             //Actually the Gate of the transistor that controls the solenoid
#define O_Button 10             //Push Button

#define I2C_ADDR 0x27           //I2C adress, you should use the code to scan the adress first (0x27) here
#define BACKLIGHT_PIN 3         // Declaring LCD Pins
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

const byte numberOfRows = 4;
const byte numberOfColumns = 3;

char keymap[numberOfRows][numberOfColumns] =
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

char keyPressed;
char code[] = {'0', '0', '0', '0'};

char checkCode1[sizeof(code)];  //Where the new key is stored
char checkCode2[sizeof(code)];  //Where the new key is stored again so it's compared to the previous one

short a = 0, i = 0, s = 0, j = 0; //Variables used later

byte rowPins[numberOfRows] = {2, 3, 4, 5};
byte columnPins[numberOfColumns] = {6, 7, 9};

bool alarmIsActivated = true;
int amountOfTries = 0;

int IRsensor = 12;
int IRstatus = 0;
bool movementDetected = false;

Keypad keypad = Keypad(makeKeymap(keymap), rowPins, columnPins, numberOfRows, numberOfColumns);

long lastTime = millis();
long lastTime2 = millis();
int delayInSeconds = 10;

void setup()
{
  Serial.begin(57600);

  pinMode(IRsensor, INPUT);

  tft.init(135, 240);
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Rtc.SetDateTime(compiled);

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);


  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  tft.setTextSize(2);
  tft.setRotation(1);

  tft.setTextSize(4);
  tft.println("SSAS");
  tft.setTextSize(1, 2);
  tft.println("Super Safe Alarm System");
  tft.println("i promise...");
  tft.println("Alarm: ACTIVATED");
  tft.setTextSize(2);
  lastTime = millis();
  tft.setCursor(0, 110);
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);

}


void loop()
{
  IRstatus = digitalRead(IRsensor);

  if (IRstatus == HIGH) {
    movementDetected = true;
    Serial.print("Movement detected");
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Enter code:");
    GetCode();
    if (a == sizeof(code)) {
      OpenDoor();
      alarmIsActivated = false;
      ShowHomeScreen();
    }
  }

  while (movementDetected) {
    if (millis() - lastTime > delayInSeconds * 1000) {
      lastTime = millis();
      ShowHomeScreen();
    }
    
    if (millis() - lastTime2 < delayInSeconds * 1000) {
      lastTime2 = millis();
      tft.setCursor(60, 200);
      RtcDateTime now = Rtc.GetDateTime();
      printDateTime(now);


      keyPressed = keypad.getKey();
      if (keyPressed == '*') {
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(0, 0);
        tft.print("Enter code:");
        GetCode();
        if (a == sizeof(code)) {
          amountOfTries = 0;
          if (alarmIsActivated == false) {
            alarmIsActivated = true;
            movementDetected = false;
            tft.setCursor(30, 53);
            tft.fillScreen(GREEN);
            tft.println("ALARM ACTIVATED");
            tft.setCursor(8, 75);
            tft.println("you have 30 seconds");
            tft.setCursor(70, 95);
            tft.println("to leave");
            delay(3000);
            for (i = 0; i < 30; i++) {
              tft.fillScreen(GREEN);
              tft.setCursor(90, 50);
              tft.setTextSize(4);
              tft.print(30 - i);
              tft.setTextSize(2);
              tft.setCursor(20, 85);
              tft.print("seconds remaining");
              delay(1000);
            }

          } else {
            OpenDoor();
            alarmIsActivated = false;
          }

        }
        else {
          amountOfTries++;
          if (amountOfTries < 3 && alarmIsActivated) {
            tft.setCursor(40, 43);
            tft.fillScreen(RED);
            tft.println("ACCESS DENIED");
            tft.setCursor(15, 60);
            tft.print(3 - amountOfTries);
            if (3 - amountOfTries == 1) {
              tft.print(" try remaining");
            } else {
              tft.print(" tries remaining");
            }
          } else if (amountOfTries == 3) {
            alarmEffects();
          }
        }
        delay(3000);
        ShowHomeScreen();

      } else if (keyPressed == '#' && !alarmIsActivated) {
        ChangeCode();
        ShowHomeScreen();

      }

      keyPressed = NO_KEY;
    }

  }



}

void GetCode() {
  i = 0;
  a = 0;
  j = 0;

  delay(1000);
  keyPressed = NO_KEY;

  while (keyPressed != '#') {
    keyPressed = keypad.getKey();
    if (keyPressed != NO_KEY && keyPressed != '#' ) {
      tft.setCursor(j, 20);
      tft.print("*");
      j = j + 15;
      if (keyPressed == code[i] && i < sizeof(code)) {
        a++;
        i++;
      }
      else
        a--;
    }
  }

}

void ChangeCode() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.print("Changing code");
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.print("Enter old code");
  GetCode();

  if (a == sizeof(code)) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.print("Changing code.");
    GetNewCode1();
    GetNewCode2();
    s = 0;
    for (i = 0 ; i < sizeof(code) ; i++) {
      if (checkCode1[i] == checkCode2[i])
        s++;
    }
    if (s == sizeof(code)) {

      for (i = 0 ; i < sizeof(code) ; i++) {
        code[i] = checkCode2[i];
      }

      tft.setCursor(30, 53);
      tft.fillScreen(GREEN);
      tft.print("CODE CHANGED");
      delay(2000);
    }
    else {
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.println("Codes are not");
      tft.print("matching.");
      delay(2000);
    }

  }

  else {
    tft.setCursor(50, 43);
    tft.fillScreen(RED);
    tft.println("WRONG CODE");
    tft.setCursor(15, 63);
    tft.print(3 - amountOfTries);
    tft.print(" tries remaining");
    delay(2000);
  }
}

void GetNewCode1() {
  i = 0;
  j = 0;
  keyPressed = NO_KEY;
  keyPressed = keypad.getKey();
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.println("Enter new code");
  tft.print("and press #");
  delay(2000);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 20);
  tft.print("and press #");

  while (keyPressed != '#') {
    keyPressed = keypad.getKey();
    if (keyPressed != NO_KEY && keyPressed != '#' ) {
      tft.setCursor(j, 0);
      tft.print("*");
      checkCode1[i] = keyPressed;
      i++;
      j = j + 15;
    }
  }
  keyPressed = NO_KEY;
}

void GetNewCode2() {
  i = 0;
  j = 0;

  keyPressed = NO_KEY;
  keyPressed = keypad.getKey();
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.println("Confirm code");
  tft.print("and press #");
  delay(2000);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 20);
  tft.print("and press #");

  while (keyPressed != '#') {
    keyPressed = keypad.getKey();
    if (keyPressed != NO_KEY && keyPressed != '#' ) {
      tft.setCursor(j, 0);
      tft.print("*");
      checkCode2[i] = keyPressed;
      i++;
      j = j + 15;
    }
  }
  keyPressed = NO_KEY;
}

void OpenDoor() {
  tft.setCursor(30, 53);
  tft.fillScreen(GREEN);
  tft.print("ACCESS GRANTED");
  delay(2000);
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.println("Alarm disabled.");
  tft.println("Welcome home!");
  delay(2000);
}

void alarmEffects() {
  while (amountOfTries == 3 && alarmIsActivated) {
    tft.fillScreen(RED);
    tft.setCursor(40, 53);
    tft.println("ACCESS DENIED");
    delay(500);
    tft.fillScreen(BLACK);
    tft.setCursor(40, 53);
    tft.println("ACCESS DENIED");
    delay(500);
  }
}

void ShowHomeScreen() {
  tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.setTextSize(4);
  tft.println("SSAS");
  tft.setTextSize(1, 2);
  tft.println("Super Safe Alarm System");
  tft.println("i promise...");
  if (alarmIsActivated) {
    tft.println("Alarm: ACTIVATED");
  } else {
    tft.println("Alarm: DISABLED");
  }
  tft.setTextSize(2);
  tft.setCursor(0, 110);
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  tft.setTextSize(2);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime & dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute() );
  tft.print(datestring);
}

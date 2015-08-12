#include <LiquidCrystal.h>
#include "DHT.h"
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//gps settings
// this pin will output the DTR signal (as set by the pc)
#define GPS_RXPIN 15 //tx on gps
#define GPS_TXPIN 16
//#define LED_PIN_VALID_SIGNAL 12 //diese led soll ein gültiges signal anzeigen





#define INTERVAL 1000

#define BUTTON_RIGHT  0
#define BUTTON_UP     1
#define BUTTON_DOWN   2
#define BUTTON_LEFT   3
#define BUTTON_SELECT 4
#define BUTTON_NONE   5

#define MENU_MIN 1
#define MENU_MAX 2
#define MENU_MEASURE 1
#define MENU_CONFIG 2

#define SUBMENU 10 //from here it is all submenus, used for moving up

#define SUBMENU_MEASURE_MIN 10
#define SUBMENU_MEASURE_MAX 13
#define SUBMENU_MEASURE_POS 10 //position in list
#define SUBMENU_MEASURE_TEMP 11
#define SUBMENU_MEASURE_DATE 12
#define SUBMENU_MEASURE_SATELLITES 13

#define SUBMENU_CONFIG_MIN 20
#define SUBMENU_CONFIG_MAX 21
#define SUBMENU_CONFIG_CURRENT 20 //position in list
#define SUBMENU_CONFIG_NEW 21
#define SUBMENU_CONFIG_EDIT 22

//CONFIGURATION/////////////////////////////////////////////
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial softwareSerial(GPS_RXPIN, GPS_TXPIN);

struct data {
  char date[11];
  char time[9];
  int year;
  byte month;
  byte day;
  byte hoD;
  byte minD;
  byte secD;
  float lat;
  float lng;
  byte numberSatellites;
  long hdop;
  float tempGround;
  float tempDevice;
};

data currentData;

long previousMillis = 0; //used in main loop for delay
uint8_t menuState = MENU_MEASURE;


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  initializeSDCard();
  Serial.begin(9600);

  //begin gps
  softwareSerial.begin(9600);

  updateDisplay();
}

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long currentMillis = millis();

  feedGPS();
  if (softwareSerial.overflow()) {
    Serial.println("Overflow");
  }

  //do this every second (interval = 1000)
  if (currentMillis - previousMillis > INTERVAL) {
    // save the last time you run the if clause
    previousMillis = currentMillis;
    Serial.print("Time: ");
    Serial.println( millis());
    Serial.print("menuState: ");
    Serial.println( menuState);

    getGPSPosition(&currentData);
    printGPSToSerial();

    feedGPS();
    getTemperature(&currentData);

    loopDisplay();
  }

  feedGPS();
  changeMenuState(getButton());
}


///////////////////////////////////////////////////
// updateDisplay
// Updates screen depending on the actual mode.
///////////////////////////////////////////////////
void updateDisplay() {
  switch (menuState) {
    case (MENU_MEASURE):
      write(1, 0, "Measure");
      break;

    case (MENU_CONFIG):
      write (1, 0, "Config");
      break;

    case (SUBMENU_MEASURE_POS):
      write("lon");
      lcd.setCursor(4, 0);
      lcd.print(currentData.lat, 6);
      overwrite(0, 1, "lat");
      lcd.setCursor(4, 1);
      lcd.print(currentData.lng, 5);
      break;

    case (SUBMENU_MEASURE_TEMP):
      write("grd");
      lcd.setCursor(4, 0);
      lcd.print(currentData.tempGround, 5);
      overwrite(0, 1, "dev");
      lcd.setCursor(4, 1);
      lcd.print(currentData.tempDevice, 5);
      break;

    case (SUBMENU_MEASURE_DATE):
      write("date");
      //      if (currentData.day < 10) {
      //        overwrite(0, 5, "0");
      //        lcd.setCursor(0, 6);
      //        lcd.print(currentData.day, 5);
      //      } else {
      //        lcd.setCursor(0, 5);
      //        lcd.print(currentData.day, 5);
      //      }
      //
      //      overwrite(0, 7, ".");
      //      if (currentData.month < 10) {
      //        overwrite(0, 5, "0");
      //        lcd.setCursor(0, 6);
      //        lcd.print(currentData.month, 5);
      //      } else {
      //        lcd.setCursor(0, 5);
      //        lcd.print(currentData.month, 5);
      //      }
      //
      //      overwrite(0, 9, ".");
      //      lcd.setCursor(0, 10);
      //      lcd.print(currentData.year, 5);

      overwrite(5, 0, currentData.date);
      overwrite(0, 1, "time");
      overwrite(5, 1, currentData.time);
      break;

    case (SUBMENU_MEASURE_SATELLITES):
      write("#sat");
      lcd.setCursor(5, 0);
      lcd.print(currentData.numberSatellites, 5);
      overwrite(0, 1, "hdop");
      lcd.setCursor(5, 1);
      lcd.print(currentData.hdop);
      break;

    case (SUBMENU_CONFIG_CURRENT):
      write("current config");
      //      char filename[14] = "";
      //      readConfig(filename);
      overwrite(0, 1, "test.txt");
      break;

    case (SUBMENU_CONFIG_NEW):
      break;

  }




}

void loopDisplay() {
  switch (menuState) {
    case (MENU_MEASURE):
    case (MENU_CONFIG):
      break;

    case (SUBMENU_MEASURE_POS):
      clearline(4, 0);
      lcd.setCursor(4, 0);
      lcd.print(currentData.lat, 5);
      clearline(4, 1);
      lcd.setCursor(4, 1);
      lcd.print(currentData.lng, 5);
      break;

    case (SUBMENU_MEASURE_TEMP):
      clearline(4, 0);
      lcd.setCursor(4, 0);
      lcd.print(currentData.tempGround, 5);
      clearline(4, 1);
      lcd.setCursor(4, 1);
      lcd.print(currentData.tempDevice, 5);
      break;

    case (SUBMENU_MEASURE_DATE):
      //      if (currentData.day < 10) {
      //        overwrite(0, 5, "0");
      //        lcd.setCursor(0, 6);
      //        lcd.print(currentData.day, 5);
      //      } else {
      //        lcd.setCursor(0, 5);
      //        lcd.print(currentData.day, 5);
      //      }
      //
      //      overwrite(0, 7, ".");
      //      if (currentData.month < 10) {
      //        overwrite(0, 5, "0");
      //        lcd.setCursor(0, 6);
      //        lcd.print(currentData.month, 5);
      //      } else {
      //        lcd.setCursor(0, 5);
      //        lcd.print(currentData.month, 5);
      //      }
      //
      //      overwrite(0, 9, ".");
      //      lcd.setCursor(0, 10);
      //      lcd.print(currentData.year, 5);
      clearline(5, 0);
      overwrite(5, 0, currentData.date);
      clearline(5, 1);
      overwrite(5, 1, currentData.time);
      break;

    case (SUBMENU_MEASURE_SATELLITES):
      clearline(5, 0);
      lcd.setCursor(5, 0);
      lcd.print(currentData.numberSatellites);
      clearline(5, 1);
      lcd.setCursor(5, 1);
      lcd.print(currentData.hdop);
      break;

    case (SUBMENU_CONFIG_CURRENT):
      write("current config");
      //      char filename[14] = "";
      //      readConfig(filename);
      overwrite(1, 0, "test.txt");
      break;

    case (SUBMENU_CONFIG_NEW):
      break;

  }

}

///////////////////////////////////////////////////
// changeMenuState
// changes the menu state depending on button pressed
///////////////////////////////////////////////////
void changeMenuState(uint8_t button) {
  if(button == BUTTON_NONE) return;
  switch (menuState) {
    case MENU_MEASURE:
    case MENU_CONFIG:
      handleButton(button, MENU_MIN, MENU_MAX);
      break;

    case SUBMENU_MEASURE_POS :
    case SUBMENU_MEASURE_TEMP :
    case SUBMENU_MEASURE_DATE :
    case SUBMENU_MEASURE_SATELLITES:
      handleButton(button, SUBMENU_MEASURE_MIN, SUBMENU_MEASURE_MAX);
      break;

    case SUBMENU_CONFIG_CURRENT:
    case SUBMENU_CONFIG_NEW :
    case SUBMENU_CONFIG_EDIT:
      handleButton(button, SUBMENU_CONFIG_MIN, SUBMENU_CONFIG_MAX);
      break;
  }
}


void handleButton(uint8_t button, uint8_t min, uint8_t max) {
  switch (button) {
    case BUTTON_UP:
      if (menuState > min) {
        menuState--;
        updateDisplay();
        Serial.println("button UP");
      }
      break;
    case BUTTON_DOWN:
      if (menuState < max) {
        menuState++;
        updateDisplay();
        Serial.println("button DOWN");
      }
      break;
    case BUTTON_LEFT:
      if (min >= SUBMENU) {
        menuState = MENU_MEASURE; //return to main menu
        updateDisplay();
      }
      Serial.println("button LEFT");
      break;
    case BUTTON_RIGHT:
    case BUTTON_SELECT:
      //TODO: depending menustate change state
      if (menuState < SUBMENU){
        menuState = menuState * 10;
        updateDisplay();
      }
      else if (min >= SUBMENU && menuState <= SUBMENU_MEASURE_MAX) { //measure submenue --> store measurement to file
        storeMeasurement(&currentData);
        updateDisplay();
      }
      
      Serial.println("button SELECT");
      break;
    default:
      break;
  }
}







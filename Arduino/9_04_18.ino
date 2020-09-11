#include <FS.h>                   //this needs to be first, or it all crashes and burns...- according to tzapu

#include "epaper_test.c"  //upload the image 128 x 296 monochrome bitmap
#include "epaper_test1.c"  //upload the image 128 x 296 monochrome bitmap
#include "epaper_test2.c"

// CUSTOMIZING CONNECTION
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
// END CUSTOMIZING CONNECTION

// SCREEN
#define OLED_RESET 4
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
// END SCREEN

// WIFI
#if defined(ESP8266)
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#else
#include <WiFi.h>          //https://github.com/esp8266/Arduino
#endif
//needed for library
#include <DNSServer.h>
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#include <WiFiManager.h> 
// END WIFI

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// VARIABLES ...
#define FIREBASE_HOST "xenon-marker-187403.firebaseio.com"
#define FIREBASE_AUTH "WGMF3ganDZtJRAy7vam9FRSLFScefuzQkheg6cZ2"
String PATH  ="users/";
String USERID = "VyhF30kc8dMpL25rNM55RMy0FKr1";
String PATH1 = "Boxes/";
String BoxNAME = "box1";
String DELIM = "/";
char NAME[100];
char UID[100];
bool shouldSaveConfig = false; //flag for saving data

// boolean to check if the paramaters should be saved
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V
//rst -> 5, busy -> 4, 

// DISPLAY
#include <GxEPD.h>
#include <GxGDEW0213Z16/GxGDEW0213Z16.cpp>  // 2.13" b/w/r                // CHOOSE THE SIZE OF THE SCREEN
//#include <GxGDEW029Z10/GxGDEW029Z10.cpp>    // 2.9" b/w/r
#include GxEPD_BitmapExamples
// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
//these are for the huzzah
//
GxIO_Class io(SPI, 0,2, 5); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io, 5, 4); // default selection of D4(=2), D2(=4)

//GxIO_Class io(SPI, SS, 0, 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
//GxEPD_Class display(io); // default selection of D4(=2), D2(=4)

// END SCREEN


void setup() {
  Serial.begin(115200);
// CUSTOMIZING CONNECTION
  Serial.println("mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
        //  if (NAME!="" && UID!=""){
          strcpy(NAME, json["NAME"]);
          strcpy(UID, json["UID"]);
        //  }       
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  
  Serial.println();
  display.init();
  
// CUSTOM
  // PARAMETERS
  WiFiManagerParameter custom_NAME("NAME", "Enter NAME", NULL, 100);
  WiFiManagerParameter custom_UID("UID", "Enter UID", NULL, 100);
 // END PARAMETERS
 // END CUSTOMIZING CONNECTION
  //wifiManager.resetSettings(); //resets wifi
  WiFiManager wifiManager;
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  if(UID!="UID" && UID!=""){
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    }
  
  wifiManager.addParameter(&custom_NAME);
  wifiManager.addParameter(&custom_UID);
//loops until it gets a configuration
  if (!wifiManager.autoConnect("FoodGuard", "12345678")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  } 
  Serial.println("Connected");
  //reads updated params
  if (custom_NAME.getValue()!="" && strlen(custom_UID.getValue())>10){
  strcpy(NAME, custom_NAME.getValue());
  strcpy(UID, custom_UID.getValue());
  }
  //save the uid and name to filesystem
 if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    Serial.println(UID);
    Serial.println(NAME);
    json["NAME"] = NAME;
    json["UID"] = UID;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
  
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  Serial.println(UID);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

// END CUSTOMIZING CONNECTION

}

void loop() {

  String theDate = getTime();

  //////////////////// date from google.com
      String theDay = theDate.substring(0,3);
      int theDayNum = theDate.substring(5,7).toInt();
      String theMonth = theDate.substring(8,11);
      int theMonthNum;
      if (theMonth=="Jan"){theMonthNum = 1;}
      else if (theMonth=="Feb"){theMonthNum = 2;}
      else if (theMonth=="Mar"){theMonthNum = 3;}
      else if (theMonth=="Apr"){theMonthNum = 4;}
      else if (theMonth=="May"){theMonthNum = 5;}
      else if (theMonth=="Jun"){theMonthNum = 6;}
      else if (theMonth=="Jul"){theMonthNum = 7;}
      else if (theMonth=="Aug"){theMonthNum = 8;}
      else if (theMonth=="Sep"){theMonthNum = 9;}
      else if (theMonth=="Oct"){theMonthNum = 10;}
      else if (theMonth=="Nov"){theMonthNum = 11;}
      else if (theMonth=="Dec"){theMonthNum = 12;};
      int theYear = theDate.substring(12,16).toInt();
///////////////////// end date from google.com

///////////////////// date from Firebase
String USERPATH = PATH + UID+DELIM + PATH1+NAME+DELIM;
String theDate1 = Firebase.getString(USERPATH+"expiration");
int theYear1 = theDate1.substring(0,4).toInt();
int theMonthNum1 = theDate1.substring(5,7).toInt();
int theDayNum1 = theDate1.substring(8,10).toInt();

///////////////////// end date from Firebase


int nd = (theYear1-theYear)*360+(theMonthNum1-theMonthNum)*30+theDayNum1-theDayNum;
Serial.println(USERPATH);
Serial.println(nd);
String name1 = Firebase.getString(USERPATH+"itemname");
String update1 = Firebase.getString(USERPATH+"updatevalue");
if(!name1.equals(update1)){
  mydisplayUpdate(nd);
  Firebase.remove(USERPATH+"updatevalue");
  Firebase.setString(USERPATH+"updatevalue",name1);
  };
  ESP.deepSleep(60e6); // 20e6 is 20 microseconds  
}

void mydisplayUpdate(int nd){
String USERPATH = PATH + UID+DELIM + PATH1+NAME+DELIM;
 display.setRotation(0);
 Serial.println("updating screen");
 if (nd > 5){
 display.setCursor(0,0);
 display.fillScreen(GxEPD_WHITE);
 display.drawExampleBitmap(gImage_epaper_test, 0, 0, 104, 212, GxEPD_WHITE);
 display.setTextColor(GxEPD_BLACK);
 }
 else if (nd > 0 && nd < 5){
 display.setCursor(0,0);
 display.fillScreen(GxEPD_WHITE);
 display.drawExampleBitmap(gImage_epaper_test1, 0, 0, 104, 212, GxEPD_RED);
 display.setTextColor(GxEPD_WHITE);
 }
 else {
 display.setCursor(0,0);
 display.fillScreen(GxEPD_WHITE);
 display.drawExampleBitmap(gImage_epaper_test2, 0, 0, 104, 212, GxEPD_BLACK);
 display.setTextColor(GxEPD_WHITE);
 };
//end pic
// screen
  const char* name = "FreeMonoBold9pt7b";
  const GFXfont* f = &FreeMonoBold9pt7b;
//  display.fillScreen(GxEPD_WHITE);
  
  display.setFont(f);
  display.setRotation(45);
  display.setCursor(0,30);
  display.println();
  //display.println("Item name:");
  String displaytext = Firebase.getString(USERPATH+"itemname").substring(0,10);
  display.println("         "+displaytext);
  //display.println("Expiration date:");
  display.println();
  display.println("         "+Firebase.getString(USERPATH+"expiration"));
  display.update();
  //display.updateWindow(0, 0, 50, 50);
  delay(8000);
  }

String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
  }
  client.print("HEAD / HTTP/1.1\r\n\r\n");
  while(!!!client.available()) {
     yield();
  }
  while(client.available()){
    if (client.read() == '\n') {    
      if (client.read() == 'D') {    
        if (client.read() == 'a') {    
          if (client.read() == 't') {    
            if (client.read() == 'e') {    
              if (client.read() == ':') {    
                client.read();
                String theDate = client.readStringUntil('\r');
                client.stop();
                //Thu, 19 Apr 2018 15:02:13 GMT
                return theDate;
              }
            }
          }
        }
      }
    }
  }
} 



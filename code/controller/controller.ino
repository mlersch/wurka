/**************************************************************************
Display controle based on https://github.com/adafruit/Adafruit_SSD1306/blob/master/examples/ssd1306_128x64_i2c/ssd1306_128x64_i2c.ino
 **************************************************************************/

#include <SPI.h> 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// Lagecy code for Menu controle
// TODO: Remove when new controle device arrives.
#define PIN_MENU_SELCET_BUTTON  12 // Taster an Pin 12
#define PIN_MENU_UP_BUTTON      11 // Taster an Pin 11
#define PIN_MENU_DOWN_BUTTON    10 // Taster an Pin 10
#define PIN_DUMMY_TEMP          2


// Settings for OLED Screen
#define SCREEN_WIDTH      128 // OLED display width, in pixels
#define SCREEN_HEIGHT     64 // OLED display height, in pixels
#define SCREEN_RESET_PIN  4 // SDA PIN of OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET_PIN);

// Settings for Temperatur and humidity sensor
#define DHTPIN 7 
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22
DHT dht(DHTPIN, DHTTYPE);

// Menu codes
// TODO: Implement
#define MENU_MODE_OPERATION 0
#define MENU_MODE_EDIT_TARGET_TEMPERATURE 1
#define MENU_MODE_EDIT_TARGET_HUMIDITY 2
#define MENU_MODE_EDIT_VENTILATION_DURATION 3
#define MENU_MODE_EDIT_VENTILATION_INTERVAL 4


// Define min and max values for each parameter 
// for the selection menu.
#define MENU_TEMPERATURE_MIN 5 // °C
#define MENU_TEMPERATURE_MAX 90 // °C
#define MENU_HUMIDITY_MIN 1 // %
#define MENU_HUMIDITY_MAX 99 // %
#define MENU_VENTILATION_DURATION_MIN 1 // Seconds
#define MENU_VENTILATION_DURATION_MAX  9999 // Seconds
#define MENU_VENTILATION_INTERVAL_MIN  1 // Minutes
#define MENU_VENTILATION_INTERVAL_MAX  2000 // Minutes


// Alarm tolerance
// How Lng a value could be beond the setting (in minutes) befor a alarm is triggered.
// TODO... TÜR-ALARM AUS DEM KELLER einbauen (Gäste WC?).
#define TIME_TO_ALARM_IN_IMUTES 5



// Define default presets for bootup
// TODO: Check how to read/write from EEPROM so after reset
// the defined vaues are still present.
int menuButton = MENU_MODE_OPERATION;
int presetTempreture           = 0;
int presetHumidity             = 0;
int presetVentilationDuration  = 90; // A Value in seconds, how long the ventilator should run.
int presetVentilationInterval  = 90; // The pause time betwene each ventilation run in minutes.

// Initial mesurement values (should be corrected during setup by the sensores)
float tempreture   = 0;
float humidity     = 0;


void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);//ventilator
  pinMode(PIN_MENU_SELCET_BUTTON, INPUT_PULLUP);      // Pin, an dem der Taster angeschlossen ist, als Eingang mit aktiviertem Pull-Up-Widerstand festlegen.
  pinMode(PIN_MENU_UP_BUTTON, INPUT_PULLUP);
  pinMode(PIN_MENU_DOWN_BUTTON, INPUT_PULLUP);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  displayLogo();
  displayMesurment();    // Draw 'stylized' characters
  
  // Temp and humidity sensor init
  dht.begin();

}

void loop() {
  //tempreture = 35.0 / 1023.0 * analogRead(PIN_DUMMY_TEMP);

  float h = dht.readHumidity(); //Luftfeuchte auslesen
  float t = dht.readTemperature(); //Temperatur auslesen
  
  // Prüfen ob eine gültige Zahl zurückgegeben wird. Wenn NaN (not a number) zurückgegeben wird, dann Fehler ausgeben.
  if (!isnan(t) && !isnan(h)) 
  {
    tempreture = (int)t;
    humidity = (int)h;
  }else{
    Serial.println("DHT22 konnte nicht ausgelesen werden");
  }

  if (digitalRead(PIN_MENU_SELCET_BUTTON) == LOW) {
    Serial.println(F("BUTTON MENU SELECT"));
    if(menuButton++ == 2){
      menuButton=0;
    }
    delay(500);
  }


  if (digitalRead(PIN_MENU_UP_BUTTON) == LOW) {
    Serial.println(F("BUTTON MENU UP"));
    if(menuButton == 1){
     if( presetTempreture++ >= 35){
      presetTempreture = 35;   
     }
    }
    if(menuButton == 2){
      Serial.println(F("SET HUMIDITY"));
     if( presetHumidity++ >= 99){
      presetHumidity = 99;   
     }
    }
    delay(200);
  }

  if (digitalRead(PIN_MENU_DOWN_BUTTON) == LOW) {
    Serial.println(F("BUTTON MENU DOWN"));
    if(menuButton == 1){
     if( presetTempreture-- <= 18){
      presetTempreture = 18;   
     }
    }
    if(menuButton == 2){
      Serial.println(F("SET HUMIDITY"));
     if( presetHumidity-- <= 40){
      presetHumidity = 40;   
     }
    }
    delay(200);
  }

  if((int)tempreture != presetTempreture){
    digitalWrite(9, HIGH);
  }else{
    digitalWrite(9, LOW);
  }
  
  displayMesurment();
}


void displayLogo(void) {
  display.setFont();
  display.clearDisplay();
  display.setTextColor(WHITE);        // Draw white text
  display.setTextSize(3);
  display.setCursor(16,21);
  display.println(F("WURKA"));
  display.display();
  display.setCursor(90,0);
  display.setTextSize(1);
  display.println(F("v1.00"));
  display.display();
  delay(2000);
}

void displayMesurment(void) {
  display.clearDisplay();
  // display.cp437(true);

  int16_t grad=248;

  display.setFont();
  display.setTextColor(WHITE);        // Draw white text
  
  display.setCursor(5,0);
  display.println(F("soll"));

  display.setCursor(60,0);
  display.println(F("ist"));

  display.setFont(&FreeSans12pt7b);

// Temperatur
  display.setCursor(5,32);
  if(menuButton == 1){
    display.setTextColor(BLACK); // Draw 'inverse' text
    display.fillRect(0, 14, 50, 21, WHITE);
  }
  display.print(presetTempreture);
  display.setFont(&FreeSans9pt7b);  
  display.print("c");

  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(WHITE);
  if((int)tempreture != presetTempreture){
    display.setCursor(110,32);
    display.print("!");
  }
  display.setCursor(60,32);
  display.print((int)tempreture);
  // display.setFont(&FreeSans9pt7b);  
  display.setFont(&FreeSans9pt7b);  
  display.print("c");


// Luftfeuchtigkeit
  display.setFont(&FreeSans12pt7b);
  display.setCursor(5,59);
  if(menuButton == 2){
    display.setTextColor(BLACK); // Draw 'inverse' text
    display.fillRect(0, 41, 50, 21, WHITE);
  }
  display.print(presetHumidity);
  display.setFont(&FreeSans9pt7b);  
  display.print("%");
  
  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(WHITE);
  if((int)humidity != presetHumidity){
    display.setCursor(110,59);
    display.print("!");
  }
  display.setCursor(60,59);
  display.print((int)humidity);
  display.setFont(&FreeSans9pt7b);  
  display.print("%");
  
  
  display.display();
}

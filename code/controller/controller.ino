/**************************************************************************
Display controle based on https://github.com/adafruit/Adafruit_SSD1306/blob/master/examples/ssd1306_128x64_i2c/ssd1306_128x64_i2c.ino
 **************************************************************************/

#include <SPI.h> 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Encoder.h>



#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// Settings for OLED Screen
#define SCREEN_WIDTH      128 // OLED display width, in pixels
#define SCREEN_HEIGHT     64 // OLED display height, in pixels
#define SCREEN_RESET_PIN  4 // SDA PIN of OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, SCREEN_RESET_PIN);

// Settings for Temperatur and humidity sensor
#define DHTPIN 7 
#define DHTTYPE DHT22 //DHT11, DHT21, DHT22
DHT dht(DHTPIN, DHTTYPE);


// Menu controler
// DT and CLK must be connected to interrupt pins for good performance (D2&D3 on UNO) 
#define MENU_CLK_PIN 2  
#define MENU_DT_PIN  3  
#define MENU_SW_PIN  4     
#define MENU_ROTARY_SENSITIVITY 2
#define MENU_ROTARY_STEP_DEVIDER 4
long MENU_ROTARY_OLD = -999;  
Encoder menuEncoder(MENU_DT_PIN, MENU_CLK_PIN);

// Menu codes
#define MENU_MAX_ITEMS 2
#define MENU_CHANGE_UP 1
#define MENU_CHANGE_DOWN 2
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


// Define device controle pins
#define DEVICE_HEATER_PIN 5
#define DEVICE_DIFFUSER_PIN 6
#define DEVICE_FAN_PIN 8


// Define default presets for bootup
// TODO: Check how to read/write from EEPROM so after reset
// the defined vaues are still present.
int MENU_SELECTED_ITEM         = MENU_MODE_OPERATION;
int presetTempreture           = MENU_TEMPERATURE_MIN;
int presetHumidity             = MENU_HUMIDITY_MIN;
int presetVentilationDuration  = MENU_VENTILATION_DURATION_MIN; // A Value in seconds, how long the ventilator should run.
int presetVentilationInterval  = MENU_VENTILATION_INTERVAL_MAX; // The pause time betwene each ventilation run in minutes.

// Initial mesurement values (should be corrected during setup by the sensores)
float tempreture   = 0;
float humidity     = 0;

int now = 0;

void setup() {
  Serial.begin(9600);

  // Menu controle
  pinMode(MENU_SW_PIN, INPUT_PULLUP);
  
  // Devices 
  digitalWrite(DEVICE_HEATER_PIN, HIGH);
  pinMode(DEVICE_HEATER_PIN, OUTPUT);  

  digitalWrite(DEVICE_DIFFUSER_PIN, HIGH);
  pinMode(DEVICE_DIFFUSER_PIN, OUTPUT);  

  pinMode(DEVICE_FAN_PIN, OUTPUT);  

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
  now = (int)millis()/1000;
  
  long newRotaryPos = menuEncoder.read()/MENU_ROTARY_STEP_DEVIDER;



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

  if(tempreture < presetTempreture){
    digitalWrite(DEVICE_HEATER_PIN, LOW);
  }else{
    digitalWrite(DEVICE_HEATER_PIN, HIGH);
  }


  if(digitalRead(MENU_SW_PIN) == LOW){
    if(MENU_SELECTED_ITEM++ == MENU_MAX_ITEMS){
      MENU_SELECTED_ITEM=0;
    }   
    delay(200);
    MENU_ROTARY_OLD = menuEncoder.read()/MENU_ROTARY_STEP_DEVIDER;
  }
  
  int change = 0;
  if(MENU_SELECTED_ITEM != 0){
   
    if(newRotaryPos - MENU_ROTARY_SENSITIVITY >= MENU_ROTARY_OLD){
      change = MENU_CHANGE_UP;
    }else if(newRotaryPos + MENU_ROTARY_SENSITIVITY <= MENU_ROTARY_OLD){
      change = MENU_CHANGE_DOWN;
    }

    if (change == MENU_CHANGE_UP) {
      if(MENU_SELECTED_ITEM == MENU_MODE_EDIT_TARGET_TEMPERATURE){
        if( presetTempreture++ >= MENU_TEMPERATURE_MAX){
          presetTempreture = MENU_TEMPERATURE_MAX;   
        }
      }
      if(MENU_SELECTED_ITEM == MENU_MODE_EDIT_TARGET_HUMIDITY){
        if( presetHumidity++ >= MENU_HUMIDITY_MAX){
          presetHumidity = MENU_HUMIDITY_MAX;   
        }
      }
    }else if (change == MENU_CHANGE_DOWN) {
      if(MENU_SELECTED_ITEM == MENU_MODE_EDIT_TARGET_TEMPERATURE){
        if( presetTempreture-- <= MENU_TEMPERATURE_MIN){
          presetTempreture = MENU_TEMPERATURE_MIN;   
        }
      }
      if(MENU_SELECTED_ITEM == MENU_MODE_EDIT_TARGET_HUMIDITY){
        if( presetHumidity-- <= MENU_HUMIDITY_MIN){
          presetHumidity = MENU_HUMIDITY_MIN;   
        }
      }
    }
    
  }
  
  if(change != 0){
    MENU_ROTARY_OLD = newRotaryPos;
  }
  displayMesurment();
  
}




void displayLogo(void) {
  display.setFont();
  display.clearDisplay();
  display.setTextColor(WHITE);        // Draw white text
  display.setTextSize(3);
  display.setCursor(18,20);
  display.println(F("WURKA"));
  display.display();
  display.setTextSize(1);
  display.setCursor(95,0);
  display.println(F("v1.00"));
  display.setCursor(10,53);
  display.println(F("by Lersch & Lersch"));
  
  display.display();
  delay(3000);
}

void displayMesurment(void) {
  display.clearDisplay();
 
  display.setFont();
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(5, 0);
  display.println(F("soll"));
  display.setCursor(60, 0);
  display.println(F("ist"));
  display.setFont(&FreeSans12pt7b);

  drawMainMenuRow(MENU_MODE_EDIT_TARGET_TEMPERATURE, presetTempreture, tempreture, 'c', 32); 
  drawMainMenuRow(MENU_MODE_EDIT_TARGET_HUMIDITY, presetHumidity, humidity, '%', 59);
  
  display.display();
}

void drawMainMenuRow(int id, int preset, int value, char unit, int rowPosition){

  display.setFont(&FreeSans12pt7b);
  display.setCursor(5, rowPosition);
  if(MENU_SELECTED_ITEM == id){
    display.setTextColor(BLACK); // Draw 'inverse' text
    display.fillRect(0, rowPosition-18, 50, 21, WHITE);
  }
  display.print(preset);
  display.setFont(&FreeSans9pt7b);  
  display.print(unit);
  
  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(WHITE);
  if((int)value != preset){
    display.setCursor(110, rowPosition);
    display.print("!");
  }
  display.setCursor(60, rowPosition);
  display.print((int)value);
  display.setFont(&FreeSans9pt7b);  
  display.print(unit);
}

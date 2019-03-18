/**************************************************************************
Display controle based on https://github.com/adafruit/Adafruit_SSD1306/blob/master/examples/ssd1306_128x64_i2c/ssd1306_128x64_i2c.ino
 **************************************************************************/

#include <SPI.h> //help
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_MENU_SELCET_BUTTON 12 // Taster an Pin 12
#define PIN_MENU_UP_BUTTON 11 // Taster an Pin 11
#define PIN_MENU_DOWN_BUTTON 10 // Taster an Pin 10
#define PIN_DUMMY_TEMP 2
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define ALARM_BLINK_SPEED 20
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int menuButton = 0; // 0=none, 1=edit Temperatur, 2=edit Luftfeuchtigkeit 
int presetTempreture = 26;
int presetHumidity   = 90;
float tempreture     = 0;

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
  

}

void loop() {
  tempreture = 35.0 / 1023.0 * analogRead(PIN_DUMMY_TEMP);

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
  display.clearDisplay();
  display.setTextColor(WHITE);        // Draw white text
  display.setTextSize(2);
  display.setCursor(10,5);
  display.println(F("Lersch"));
  display.display();
  delay(500);
  display.setCursor(20,25);
  display.println(F("Climate"));
  display.display();
  delay(500);
  display.setCursor(30,45);
  display.println(F("Control"));
  display.display();
  delay(500);
  display.setCursor(90,0);
  display.setTextSize(1);
  display.println(F("v1.00"));
  display.display();
  delay(2000);
}

void displayMesurment(void) {
  display.clearDisplay();
  display.cp437(true);

  int16_t grad=248;

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  
   display.setCursor(5,0);
  display.println(F("soll"));

  display.setCursor(70,0);
  display.println(F("ist"));


  display.setTextSize(3.0);
// Temperatur
  display.setCursor(5,15);
  if(menuButton == 1){
    display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  }
  display.print(presetTempreture);
  display.write(grad);

  display.setTextColor(WHITE);
  display.setCursor(70,15);
  display.print((int)tempreture);
  display.write(grad);


// Luftfeuchtigkeit
  display.setCursor(5,42);
  if(menuButton == 2){
    display.setTextColor(BLACK, WHITE); // Draw 'inverse' text
  }
  display.print(presetHumidity);
  display.print("%");
  
  display.setTextColor(WHITE);
  display.setCursor(70,42);
  display.print(F("92%"));
  
  
  display.display();
}

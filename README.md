# wurka
## Arduino controlled WuRKa

Dies ist eine Arduino basierte Schaltung zur Steureung von Luftfeuchtigkeit und Temperatur in einem Wurstreifeschrank.


Das __Fritzing__ basierte Schaltungslayout findest du im Ordner `/fritzing`.
Der Schaltplan ist nicht identisch mit der aktuell laufenden implementierung, sollte aber kompatibel mit dem Code sein.


Den __Code__ für den Arduino findest du im Ordner `/code/controller`


 # Technik
 
 ## Aktuell verwendete Komponenten:
 * Arduino UNO
 * DHT22 / AM2302 Luftfeuchte und Temperatursensor
 * 0,96 Zoll I2C IIC SPI 128 x 64 Monocrom OLED Display
 * Inkrementalgeber mit Taster (Hikig KY-040 Rotary Encoder)
 * 4 Kanal DC 5V Relaismodul mit Optokoppler
 * Optokoppler für Ansteuerung des Luftbefeuchters.

## Geplante Weiterentwicklung:
 * Messdaten per WLAN (ESP8266) an Logging-Server für Statistiken
 * größeres 128x128px RGB OLED Display
 * Erweitertes Menü mit mehr einstellmöglichkeiten
 * Atmel ATmega328P ohne Arduino development board... und
 * Komplettes Platinenlayout auf einem gefertigten PCB
 * Alarm per Mobiltelefon bei abweichenden Messwerten
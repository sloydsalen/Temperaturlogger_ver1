#include <Arduino.h>
#include <SPI.h>
#include<SD.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>;


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Bluetooth 
SoftwareSerial bluetooth(4, 5); //Bluetooth RX, TX
const byte BTState = 6;         // Bluetooth STATE

/*SDkortet har denne konfigurasjonen
MOSI - pinne D11
MISO - pinne D12
CS -- pinne 9
*/

//vi har koblet DS18b20 til pinne D2 på arduinoen

#define DS18B20 2

OneWire ourWire(DS18B20);
DallasTemperature sensors(&ourWire);
const int chipSelect=9;

void setup()
{
  // we need to read from bluetooth state pin
  pinMode(BTState, INPUT);

  // serial com - cable connected to pc AND bluetooth
  Serial.begin(9600);
  bluetooth.begin(9600);
  delay(1000);

  // Dallas Temperature sensor init
  sensors.begin();

  // initialize the LCD
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Hello, world!");
  
  // SD card initialization
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
   // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("SD card initialized.");

  
}



void loop()
{
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // get sensor value
  sensors.requestTemperatures();
  float datavalue = sensors.getTempCByIndex(0);

  Serial.print(datavalue);
  Serial.println(" grader");
  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(datavalue);
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  // send data over bluetooth
  bool connected = (digitalRead(BTState) == HIGH);
  if (connected) {
    bluetooth.println(datavalue);
  }
  else {
    Serial.println("WARNING. not connected wia bluetooth");
  }
  
  lcd.clear();
  // write temp to lcd
  lcd.setCursor(0,0);
  lcd.print("T=");
  lcd.print(datavalue, 1);
  lcd.print("C");

  // write time info to lcd
  
  // write bluetooth connection status to lcd
  lcd.setCursor(0,1);
  if (connected) {
    lcd.print("Connected!");
  }else{
    lcd.print("No connection...");
  }

  delay(1000);
} 

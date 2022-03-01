#include <dhtnew.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
//I2C ADDRESS OF SCREEN: "0x3C"

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);


DHTNEW sensor(5);
float humidity = 0;
float temp = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

}

void loop() {
  display.clearDisplay();
  
  int chk = sensor.read();
  // put your main code here, to run repeatedly:
  humidity = sensor.getHumidity();
  temp = sensor.getTemperature();
  display.setCursor(0,0);
  Serial.println(humidity);
  display.println("H: " + String(humidity) + "%");
  
  display.setCursor(0,8);
  display.println("T: " + String(temp) + "C");
  display.display();
  
  delay(10000);

}

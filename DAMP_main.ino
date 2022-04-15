//TODOs:
//Need to implement a buffer (risk of getting in a loop with hum/dehum), in general make the conditioning algorithm better
//Draw the case
//Bugfixes (set delays correctly, input not always working, default +5 on humidity when in setup)
//Draw the high voltage circuit

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHT_PIN 5
#define DHT_TYPE DHT21 
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C

#define LEFT_BUTTON 8
#define CENTER_BUTTON 9
#define RIGHT_BUTTON 10

#define HUMIDIFIER 11
#define DEHUMIDIFIER 12

#define DEFAULT_H_TARGET 50

//I2C ADDRESS OF SCREEN: "0x3C"

void splashScreen();
void startPreserver(int h_target);
float updateTempHum();
void drawOperationGUI(int h_target);
int settings(int h_target);
void drawSetupGUI(int h_target);
void clearHum();

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
DHT sensor(DHT_PIN, DHT_TYPE);

float humidity = 0;
float temp = 0;
int h_target = DEFAULT_H_TARGET;

//void Loop global vars
int lastCenterButtonState = 0, centerButtonState = 0, lastRightButtonState = 0, rightButtonState = 0;

void setup() {

  pinMode(LEFT_BUTTON, INPUT);
  pinMode(CENTER_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);

  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(DEHUMIDIFIER, OUTPUT);
  digitalWrite(HUMIDIFIER, LOW);
  digitalWrite(DEHUMIDIFIER, LOW);

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  sensor.begin();
  splashScreen();

  
}

void loop() {
    delay(100);
    updateTempHum();
    
    centerButtonState = digitalRead(CENTER_BUTTON);
    rightButtonState = digitalRead(RIGHT_BUTTON);

    if(centerButtonState != lastCenterButtonState){
        if(centerButtonState == HIGH){
            startPreserver(h_target);
            digitalWrite(HUMIDIFIER, LOW);
            digitalWrite(DEHUMIDIFIER, LOW);
            splashScreen();
            delay(100);
        }

    }

        
    if(rightButtonState != lastRightButtonState){
        if(rightButtonState == HIGH){
            h_target = settings(h_target);
            splashScreen();
            delay(100);
        }
    }

    
  lastCenterButtonState = centerButtonState;
  lastRightButtonState = rightButtonState;
}

void splashScreen(){
  display.clearDisplay();
  display.drawRect(0, 0, 128, 10, SSD1306_WHITE);
  display.fillRect(0, 0, 128, 10, SSD1306_WHITE);
  updateTempHum();

  display.setCursor(39, 2);
  display.setTextColor(SSD1306_BLACK);
  display.print("DAMP v0.1");

  display.drawTriangle(58, 20, 58, 34, 70, 27, SSD1306_WHITE);
  display.fillTriangle(58, 20, 58, 34, 70, 27, SSD1306_WHITE); 
  display.drawCircle(63, 27, 12, SSD1306_WHITE);

  display.setCursor(50, 44);
  display.setTextColor(SSD1306_WHITE);
  display.print("Start");

  display.drawCircle(105, 29, 10, SSD1306_WHITE);

  //Drawing "gear"
  display.drawCircle(105, 29, 4, SSD1306_WHITE);
  display.fillCircle(105, 29, 4, SSD1306_WHITE);
  display.drawCircle(105, 29, 2, SSD1306_BLACK);
  display.fillCircle(105, 29, 2, SSD1306_BLACK);
  display.setCursor(92, 44);
  display.print("Setup");
  
  


  updateTempHum();
  return;
}

void startPreserver(int h_target){
  float humidity;
  bool stop = false;

  drawOperationGUI(h_target);

  while(!stop){
    humidity = updateTempHum();
    
    if(humidity > h_target+2.5){
      digitalWrite(HUMIDIFIER, LOW);
      digitalWrite(DEHUMIDIFIER, HIGH);
    }else{
      if(humidity < h_target-2.5){
        digitalWrite(HUMIDIFIER, HIGH);
        digitalWrite(DEHUMIDIFIER, LOW);
      }else{
        digitalWrite(HUMIDIFIER, LOW);
        digitalWrite(DEHUMIDIFIER, LOW);
      }
    }

    if (digitalRead(LEFT_BUTTON)==HIGH){ 
      stop = true;
    }
  }
  
  return;
}

float updateTempHum(){
  temp = sensor.readTemperature();
  humidity = sensor.readHumidity();

  display.drawRect(0, 54, 128, 10, SSD1306_WHITE);
  display.fillRect(0, 54, 128, 10, SSD1306_WHITE);

  display.setCursor(4, 55);
  display.setTextColor(SSD1306_BLACK);
  display.print("T: " + String(temp) + "C  H: " + String(humidity) + "%");
  display.display();

  return humidity;
}

void drawOperationGUI(int h_target){

  /*Draws upper and lower rectangle (name + temp and hum)*/
  display.clearDisplay();
  display.drawRect(0, 0, 128, 10, SSD1306_WHITE);
  display.fillRect(0, 0, 128, 10, SSD1306_WHITE);

  display.setCursor(39, 2);
  display.setTextColor(SSD1306_BLACK);
  display.print("DAMP v0.1");
  updateTempHum();

  /*Draws Stop control*/
  display.drawCircle(25, 27, 12, SSD1306_WHITE);

  display.setCursor(14, 44);
  display.setTextColor(SSD1306_WHITE);
  display.print("Stop");


  display.drawRect(18, 20, 15, 15, SSD1306_WHITE);
  display.fillRect(18, 20, 15, 15, SSD1306_WHITE);

  display.setCursor(50, 27);
  display.setTextColor(SSD1306_WHITE);
  display.print("h_target: " + String(h_target) +"%");

  
  return;
}

int settings(int h_target){
    bool stop = false;
    int  leftButtonState = 0, lastLeftButtonState = 0, rightButtonState = 0, lastRightButtonState = 0;
    drawSetupGUI(h_target);


    while(!stop){
      rightButtonState = digitalRead(RIGHT_BUTTON);
      leftButtonState = digitalRead(LEFT_BUTTON);
      updateTempHum();

      //hum++
      if (rightButtonState != lastRightButtonState) {
        if(rightButtonState == HIGH){
          if(h_target < 100)
            h_target += 5;
          
          clearHum();
          display.setCursor(58, 30);
          display.setTextColor(SSD1306_WHITE);
          display.print(String(h_target) + "%");
        }
        delay(50);
      }

     //hum--
     if (leftButtonState != lastLeftButtonState) {
        if(leftButtonState == HIGH){
          if(h_target > 0)
            h_target -= 5;

          clearHum();
          display.setCursor(58, 30);
          display.setTextColor(SSD1306_WHITE);
          display.print(String(h_target) + "%");
        }
        delay(50);
      }

      if (digitalRead(CENTER_BUTTON)==HIGH){ 
        stop = true;
      }

      lastRightButtonState = rightButtonState;
      lastLeftButtonState = leftButtonState;
  }
  

   return h_target;
}



void drawSetupGUI(int h_target){
  display.clearDisplay();
  display.drawRect(0, 0, 128, 10, SSD1306_WHITE);
  display.fillRect(0, 0, 128, 10, SSD1306_WHITE);

  display.setCursor(39, 2);
  display.setTextColor(SSD1306_BLACK);
  display.print("DAMP v0.1");
  updateTempHum();

  //Center circle
  display.drawCircle(65, 33, 16, SSD1306_WHITE);
  display.setCursor(58, 30);
  display.setTextColor(SSD1306_WHITE);
  display.print(String(h_target) + "%");
  
  //Right circle
  display.drawCircle(105, 33, 8, SSD1306_WHITE);
  display.setCursor(103, 30);
  display.print("+");

  //Left circle
  display.drawCircle(25, 33, 8, SSD1306_WHITE);
  display.setCursor(23, 30);
  display.print("-");

  return;
}

void clearHum(){
  display.fillCircle(65, 33, 12, SSD1306_BLACK);
  return;
}


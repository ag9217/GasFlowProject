#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <ServoTimer2.h> // to avoid library conflict 
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal.h>

#define REPORTING_PERIOD_MS   1000
const int rs = 9, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int buttonState1 = 0;
int buttonState2 = 0;

ServoTimer2 motor;
//int inpin = 7; // the arduino get the signal from pulse oximeter at pin 7
int val = 0; // store the current oxygen saturation level
int angle = 750; //750 is 0 degree in the library 
int rotation_inc = 0;
int rotation_dec = 0; // 125 means 15 degree in the library
int sf = 10; // safety factor 
int inPin = 11;
int8_t buf=98;
const char *valmsg;
const char *msg = "BD";
bool check1 = false;
bool check2 = false;
RH_ASK driver;
PulseOximeter pox;

uint32_t tsLastReport1 = 0;
uint32_t tsLastReport2 = 0;
uint32_t tsLastReport3 = 0;


void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    lcd.begin(16,2);
    lcd.clear();
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
    pinMode(inPin, OUTPUT);
    motor.attach(inPin);
    Serial.println("Initializing MAX30100");
    // Initialize the PulseOximeter instance and register a beat-detected callback
    pox.begin();
    pox.setOnBeatDetectedCallback(onBeatDetected);
    angle =750;
    rotation_inc = 125;
    rotation_dec = 125;
    pinMode(6, INPUT);
    pinMode(8, INPUT);
}

void loop()
{
    
  //  uint8_t buf[1]; // ideal oxygen saturation level 
  //  uint8_t buflen = sizeof(buf);
  //  if (driver.recv(buf, &buflen)) // Non-blocking
  //  {
  //    int i;
  //    // Message with a good checksum received, dump it.
  //    //Serial.print("Message: ");
  //    Serial.println((char*)buf);         
  //  }
  buttonState1 = digitalRead(6);
  buttonState2 = digitalRead(8);
  if (buttonState1 == LOW) check1 = true;
  if(buttonState1 == HIGH && check1)
  {
    buf++;
    check1 = false;
    rotation_inc = 0;
    rotation_dec = 250;
    Serial.print(int(buf));
  }
  if (buttonState2 == LOW) check2 = true;
    if(buttonState2 == HIGH && check2)
  {
    buf--;
    check2=false;
    rotation_dec = 250;
    rotation_inc = 0;
    Serial.print(int(buf));
  }

   pox.update();
   if (millis() - tsLastReport1 > REPORTING_PERIOD_MS) {
        lcd.clear();
        lcd.print("SpO2: ");
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        
        Serial.print("bpm / SpO2:");
        val = pox.getSpO2();
        Serial.print(val);
        lcd.print(val);
        lcd.print("%");
       

        lcd.setCursor(0,1);
        lcd.print("Target: ");
        lcd.print(buf);
        lcd.print("%");

        if(val == 94){valmsg = "94";} if(val == 95){valmsg = "95";} if(val == 96){valmsg = "96";} if(val == 97){valmsg = "97";} 
        if(val == 98){valmsg = "98";} if(val == 99){valmsg = "99";} if(val == 93){valmsg = "93";} if(val == 92){valmsg = "92";}
        if(val == 0){valmsg = "00";}
        driver.send((uint8_t*)valmsg, strlen(valmsg)); //send message to pager
        
        //Serial.print(pox.getSpO2());
        Serial.print("% / temp:");
        //Serial.print(pox.getTemperature());
        //Serial.println("C");

        tsLastReport1 = millis();
    }//just for test 
   
    val = pox.getSpO2(); 
    
    if (abs(val-int(buf)) < sf && (angle+rotation_inc<2250) && (angle+rotation_dec>750) && (millis() - tsLastReport2 > REPORTING_PERIOD_MS))
    {
      tsLastReport2 = millis();
      if ((val-int(buf)) < 0 && abs(val-int(buf)) < sf) 
      {
        motor.write(angle+rotation_inc);
        //delay(500);
        
        rotation_inc+=125;
        //if (angle+rotation>=2250) break;
        Serial.println(rotation_inc); // just for test
        Serial.println(angle+rotation_inc); // just for test
      }
      else if ((val-int(buf)) > 0 && abs(val-int(buf)) < sf)
      {
        motor.write(angle+rotation_dec);
        //delay(500);
        //if (angle+rotation>750) 
        rotation_dec-=125; 
        Serial.println(rotation_dec); // just for test
        Serial.println(angle+rotation_dec); // just for test
      }
      
    }
    if (abs(val-(int)buf) > sf && (millis() - tsLastReport3 > REPORTING_PERIOD_MS))
      {
          
//          driver.send((uint8_t*)msg, strlen(valmsg));
          
//          Serial.print("BABY DEAD");  
          //lcd.print(val);
          //lcd.setCursor(0,1);
          tsLastReport3 = millis();
      }
    
}

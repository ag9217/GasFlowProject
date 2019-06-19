#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <ServoTimer2.h> // to avoid library conflict 
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"  // library needed for the pulseoximeter
#include <LiquidCrystal.h>

#define REPORTING_PERIOD_MS   1000  // this is the time delay we want to add. lcd screen does not like delay function, in order to add delay in the lcd screen, we use millis function to create time delay. 
const int rs = 9, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // set up parameter for lcd screen
int buttonState1 = 0;
int buttonState2 = 0;

ServoTimer2 motor; // ServoTimer2 is the parameter type defined in the library
int val = 0;  // store the current oxygen saturation level
int angle = 750; //750 is 0 degree in the library, 2250 is 180 degree which is also the maximum angle, this parameter is used to record the current angle of the valve
int rotation_inc = 0;
int rotation_dec = 0; // 125 means 15 degree in the library, these two parameters are used to record the how many degrees the valve need to change later
int sf = 10; // safety factor 
int inPin = 11;
int8_t buf=98; // this parameter represents the desired oxygen saturation level
const char *valmsg;
const char *msg = "BS"; // BS stands for baby situation, it means that doctors need to check the baby
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
    pinMode(6, INPUT);  // set up the pin for the two bottons
    pinMode(8, INPUT);
}

void loop()
{
  buttonState1 = digitalRead(6);
  buttonState2 = digitalRead(8);
  if (buttonState1 == LOW) check1 = true; 
  if(buttonState1 == HIGH && check1)  // the desired oxygen saturation level can be increased by pressing botton 1
  {
    buf++;
    check1 = false;
    rotation_inc = 0;
    rotation_dec = 250;
    Serial.print(int(buf));
  }
  if (buttonState2 == LOW) check2 = true;
  if(buttonState2 == HIGH && check2)  // the desired oxygen saturation level can be decreased by pressing botton 2
  {
    buf--;
    check2=false;
    rotation_dec = 250;
    rotation_inc = 0;
    Serial.print(int(buf));
  }

   pox.update();
   if (millis() - tsLastReport1 > REPORTING_PERIOD_MS) 
   {
        lcd.clear();
        lcd.print("SpO2: ");   // print the current oxygen saturation level on lcd screen
        val = pox.getSpO2();
        lcd.print(val);
        lcd.print("%");
       

        lcd.setCursor(0,1);
        lcd.print("Target: "); // print the desired oxygent saturation level on lcd screen
        lcd.print(buf);
        lcd.print("%");

        if(val == 94){valmsg = "94";} if(val == 95){valmsg = "95";} if(val == 96){valmsg = "96";} if(val == 97){valmsg = "97";} 
        if(val == 98){valmsg = "98";} if(val == 99){valmsg = "99";} if(val == 93){valmsg = "93";} if(val == 92){valmsg = "92";}
        if(val == 0){valmsg = "00";}
        driver.send((uint8_t*)valmsg, strlen(valmsg)); //send the current oxygen saturation level to pager
        

        tsLastReport1 = millis();  // this is how the time delay is created
    } 
   
    val = pox.getSpO2();  // update the current oxygen saturation level 
    
    if (abs(val-int(buf)) < sf && (angle+rotation_inc<2250) && (angle+rotation_dec>750) && (millis() - tsLastReport2 > REPORTING_PERIOD_MS)) // if the current oxygen saturation level is lower than the desired oxygen saturation level 
    {                                                                                                                                        // and the rotation angle of the valve is within the range (0-180 degree)
      tsLastReport2 = millis();
      if ((val-int(buf)) < 0 && abs(val-int(buf)) < sf) 
      {
        motor.write(angle+rotation_inc);
        
        rotation_inc+=125;   //the valve will increase the rotation angle by 15 degree at a time
      }
      else if ((val-int(buf)) > 0 && abs(val-int(buf)) < sf) // if the current oxygen saturation level is higher than the desired oxygen saturation level
      {
        motor.write(angle+rotation_dec);
        rotation_dec-=125;   // the valve will decrease the rotation angle by 15 degree at a time
      }
      
    }
    if (abs(val-(int)buf) > sf && (millis() - tsLastReport3 > REPORTING_PERIOD_MS)) // if the difference between the current oxygen saturation level and the desired oxygen saturation level is larger than the safety factor
    {
          
        driver.send((uint8_t*)msg, strlen(valmsg));  // a message is sent to pager to warn the doctor
          
        Serial.print("SPO2 LOW");  
        lcd.print(val);
        lcd.setCursor(0,1);
        tsLastReport3 = millis();
    }
    
}

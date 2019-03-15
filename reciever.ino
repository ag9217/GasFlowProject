#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <LiquidCrystal.h>

RH_ASK driver;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 9, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void loop()
{
  
    //receiver code
    uint8_t buf[12];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
      int i;
      // Message with a good checksum received, dump it.
      Serial.print("Message: ");
      Serial.println((char*)buf);         
    }
    lcd.print("SpO2: ");
    for(int i = 0; i < 2; i++)
    {
      lcd.print(char(buf[i]));
    }
    lcd.print("%");
    delay(1000);
    lcd.clear();

    //transmitter code

//      Serial.print((char*)buf);
//      delay(1000);
//      lcd.clear();

      
    //}
}

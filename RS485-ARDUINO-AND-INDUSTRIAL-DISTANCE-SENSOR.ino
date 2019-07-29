//********************************************************************************//
//  Name    : RS485 - Arduino and Laser Sensor                                    //
//  Author  : TUENHIDIY                                                           //
//  Date    : 27 July 2019                                                        //
//  Version : 0.0                                                                 //
//  Notes   : Arduino UNO R3 communicate with distance laser sensor through RS485 //
//          : The distance value is shown on LoLShield screeen                    //
//********************************************************************************//

#include <avr/pgmspace.h>
#include "Charliplexing.h"
#include "Myfont.h"
#include "Font.h"

#include <HardwareSerial.h>
#define BAUDRATE 9600

// Font 3x5 for the digits
int digits[][15] = {
   {1,1,1,1,0,1,1,0,1,1,0,1,1,1,1}
  ,{0,0,1,0,0,1,0,0,1,0,0,1,0,0,1}
  ,{1,1,1,0,0,1,1,1,1,1,0,0,1,1,1}
  ,{1,1,1,0,0,1,1,1,1,0,0,1,1,1,1}
  ,{1,0,1,1,0,1,1,1,1,0,0,1,0,0,1}
  ,{1,1,1,1,0,0,1,1,1,0,0,1,1,1,1}
  ,{1,0,0,1,0,0,1,1,1,1,0,1,1,1,1}
  ,{1,1,1,0,0,1,0,0,1,0,0,1,0,0,1}
  ,{1,1,1,1,0,1,1,1,1,1,0,1,1,1,1}
  ,{1,1,1,1,0,1,1,1,1,0,0,1,0,0,1}
};                           

void setup() 
{
  LedSign::Init(DOUBLE_BUFFER);
  Serial.begin(BAUDRATE,SERIAL_8N1);  //Using Serial port with 9600 baudrate, non-parity, 8 data bits, 1 stop bit.
  pinMode(A1, OUTPUT);                //DE/RE Controling pin of RS-485
  pinMode(A0, OUTPUT);                //LED Indicator
}
void loop() 
{
  byte getdata[2];
  byte High_Byte;
  byte Low_Byte;
  unsigned int wordx;
  boolean D7=0;
  // Enable Received 
  digitalWrite(A1,LOW);     //DE/RE=LOW Receive Enabled
  // Receive data from Laser Sensor
  if(Serial.available())
    { 
      digitalWrite(A0,HIGH);//LED
      for (byte i=0; i<2; i++)
        {
          getdata[i]=Serial.read();
          delay(15);
        }     
    }
  else
    {
    digitalWrite(A0,LOW);   //LED 
    LedSign::Clear();
    Myfont::Draw(0, 'F');
    Myfont::Draw(9, 'F');
    delay(1000);
    LedSign::Clear();    
    }    
  // Check the correct data
if(Serial.available())
  {  
  if (bitRead(getdata[0],0)==0)
    {
      Low_Byte=getdata[0];
    }
  if (bitRead(getdata[1],0)==1)
    {
      High_Byte=getdata[1];
    }
    
  // Two bytes conversion
  // | 0 0 D11 D10 D9 D8 D7 1 | | D6 D5 D4 D3 D2 D1 D01 0 |
  // |        High_Byte       | |       Low_Byte          |
  
  D7 = bitRead(High_Byte,1);
  Low_Byte=(Low_Byte & 0xFE) >> 1;
  bitWrite(Low_Byte, 7, D7);
  High_Byte=(High_Byte & 0x3F) >> 2;
  wordx = (High_Byte << 8) | Low_Byte;

  // Print all digits on LoLShield screeen.
  if(wordx < 250)
    {
      LedSign::Clear();
      Myfont::Draw(0, 'L');
      Myfont::Draw(9, 'L');
    }
  else if ((wordx > 250)& (wordx < 4020))
  {
  LedSign::Clear();
  // first digit - thousand
  for (int i=0; i<15; i++)
  {
    LedSign::Set((i % 3)+ 0,(i/3)+2, digits[(wordx/1000) % 10][i]);
  }
  // second digit - hundred
  for (int i=0; i<15; i++)
    {
      LedSign::Set((i % 3)+ 4,(i/3)+2, digits[(wordx/ 100) % 10][i]);
    }
  // third digit - Ten
  for (int i=0; i<15; i++)
    {
      LedSign::Set((i % 3)+ 8,(i/3)+2, digits[(wordx/10) %10][i]);
    }
  // fourth digit - Unit
  for (int y=0; y < 9; y++) 
    {
      if (y < (wordx %10)) 
        {
          LedSign::Set(13, y, 1);        // set the LED on
        } 
      else 
        {
          LedSign::Set(13, y, 0);       // set the LED off
        }
    }
  }
  else
  {
    LedSign::Clear();
    Myfont::Draw(0, 'H');
    Myfont::Draw(9, 'H');
  }
}
  LedSign::Flip();
}

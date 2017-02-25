#include "ADNS3080.h"
#include<Wire.h>

ADNS3080 of;
float heading = 0;
char outMessage[6];
bool setPosFlag = false;

void setup() 
{  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
//  Serial2.begin(9600);
  if(of.mousecam_init()==-1)
  {
    Serial.println("Mouse cam failed to init");
//    Serial2.println("Mouse cam failed to init");
    while(1);
  }  
  else
  {
    Serial.println("way to go man");
  }
}

void loop() 
{
  of.update(heading);
}

void receiveEvent(int howMany)
{
  char a = 0;
  int i = 0;
  byte message[7];
  while(Wire.available()) //supposed to receive the current heading of the rover
  {
    a = Wire.read();
    message[i] = a;
    i++;
  }
  heading = message[1] * 10 + message[0]; //now the heading is from 0 - 360
  if(heading > 180 && heading < 360)
  {
    heading -= 360;
  }
  heading *= PI / 180.0;
  setPosFlag = message[6];
  if(setPosFlag)
  {
    of.X = (float)(message[3] * 10 + message[2]) / 100.0;
    of.Y = (float)(message[5] * 10 + message[4]) / 100.0;
  }
}

void requestEvent()
{
  byte outMessage[6];
  int x,y;
  boolean flagx,flagy;
  if(of.X < 0)
  {
    flagx = 1;
    x = (int)(of.X * 100) * -1;
  }
  else
  {
    flagx = 0;
    x = (int)(of.X * 100);
  }
  if(of.Y < 0)
  {
    flagy = 1;
    y = (int)(of.Y * 100) * -1;
  }
  else
  {
    flagy = 0;
    y = (int)(of.Y * 100);
  }
 
  outMessage[1] = x / 10;
  outMessage[0] = (x - outMessage[1] * 10);
  outMessage[2] = flagx;
  
  outMessage[4] = y / 10;
  outMessage[3] = (y - outMessage[4] * 10);
  outMessage[5] = flagy;
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(outMessage[3]);
  Serial.print("\t");
  Serial.print(outMessage[4]);
  Serial.print("\t");
  Serial.print(heading);
  Serial.print("\t");
  Serial.println(of.squal);
  Wire.write(outMessage,6); //sending 4 bytes containing x and y position
}


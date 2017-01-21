
#include <Arduino.h>
#include <String.h>
#include <stdlib.h>
#include <math.h>

byte numbers[] = {
  0b11111100,0b00100100,0b01111001,0b01101101,0b10100101,0b11001101,0b11011101,0b01100100,0b11111101,0b11100101,0b00000000};
//0b fabedc(dp)g
//byte segments[] = {0b00000001,0b00000010,0b00000100,0b00001000,0b00010000,0b00100000,0b01000000,0b10000000,0b00000000};
byte digits[] = {
  0b00000001,0b00000010,0b00000100,0b00001000,0b00010000,0b00000000};
int displays[] = {
  3,1,3,3,7};
boolean decimals[] = {
  0,0,0,1,0};
int dataPin = 4;
int enablePin = 5;
int latchPin = 6;
int clockPin = 7;
int resetPin = 8;
String command = "";

int cpu = 9;
int mem = 10;
int cpu_val =128;
int mem_val = 128;
//Pins
const int fan_pwm = 3;
const int TPIN1 = A5;

//Etc
float goal = 80.0;//f
float vcc = 5.00;//voltage
float R = 98900.0;//other resistor
float beta = 4036.0;
float thermistor = 100000.0;

//PID
double Kp = 5.0;
double Ki = 5.0;
double Kd = 4.0;
float epsilon = 0.0;

//No need to edit beyond here
double I = 0.0;
double Pp = 0.0;
unsigned long Tp;

//Calculate temperature in Fahrenheit
double Thermistor(int value) {
  return ((1.0/((1.0/298.15) + (1.0 / beta) * log(((R * 1024.0 / value) - R) / thermistor))) - 273.15) * (9.0 / 5.0) + 32.0;               
}

//Calculate PID value
double PID(double P) {

  if (abs(P) < epsilon) {
    I = 0.0;
    Pp = P;
    return 0;
  }

  unsigned long T = millis();
  double dT = T-Tp;
  Tp = T;

  I += P * (dT/1000.0);

  double D = (P - Pp) / (dT/1000.0);
  Pp = P;

  return (Kp*P + Ki*I + Kd*D);
}

//PWM the fans
void setOutput(double value) {
  int mode = analogRead(A4);
  int fan_pot = analogRead(A6);
  if(mode < 600)
  {
    analogWrite(fan_pwm, 255);  
  }
  else if(mode < 700)
  {
    if (value > 0.0) {
      analogWrite(fan_pwm, 0);
    } 
    else {
      if (value < -255.0) value = -255.0;
      analogWrite(fan_pwm, map((value * -1.0),0,255,150,255));  
    }
  }
  else
  {
    analogWrite(fan_pwm, map(fan_pot,0,1023,0,255));  
  }
}

byte start[] = {
  0x53,0x54,0x41,0x52,0x54};

void serialEvent() {

  //Wait for start of data (deliminated by start[[] sent from pc)
  if (Serial.available() < 21)
    return;
  for (int i=0; i<sizeof(start)/sizeof(start[0]); ++i) 
    if (Serial.read() != start[i])
      i=0;

  //Wait until we have all data
  //while (Serial.available() < 16);

  for(int i=0;i<5;i++)
  {      
    displays[i] = (Serial.read()-0x30);

    if(Serial.read() == 0x2E)
    {
      decimals[i] = 1;
    }
    else
    {
      decimals[i] = 0;
    }
  }
  cpu_val = 0;
  cpu_val += ((Serial.read()-0x30)*100);
  cpu_val += ((Serial.read()-0x30)*10);
  cpu_val += ((Serial.read()-0x30)*1);
  mem_val = 0;
  mem_val += ((Serial.read()-0x30)*100);
  mem_val += ((Serial.read()-0x30)*10);
  mem_val += ((Serial.read()-0x30)*1);
}


void setup()
{
  Serial.begin(115200);
  //Serial.println("begin");
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin,LOW);
  digitalWrite(resetPin,HIGH);
  pinMode(fan_pwm, OUTPUT);
  Tp = millis();
}

void loop()
{
  for (int digitToDisplay = 0; digitToDisplay < 6; digitToDisplay++) {
    digitalWrite(latchPin, LOW);
    if(decimals[digitToDisplay])
    {
      shiftOut(dataPin, clockPin, MSBFIRST, (numbers[displays[digitToDisplay]]|0b00000010));       
    }
    else
    {
      shiftOut(dataPin, clockPin, MSBFIRST, numbers[displays[digitToDisplay]]); 
    }
    shiftOut(dataPin, clockPin, MSBFIRST, digits[digitToDisplay]); 
    digitalWrite(latchPin, HIGH);
  }
  setOutput(PID(goal - Thermistor(analogRead(TPIN1))));
  //set analog stuff
  analogWrite(cpu,(cpu_val));
  analogWrite(mem,(mem_val));
}


//www.elegoo.com
//2016.12.09

#include<Wire.h>
// Arduino pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output

int buzzer = 12;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t GyX,GyY;

int incomingBuzzer;

void setup() {
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  pinMode(buzzer,OUTPUT);//initialize the buzzer pin as an output
  
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
  
}

void loop() {
  unsigned int i;
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x43);  // starting with register 0x43 (GYRO_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,4,true);  // request a total of 4 registers
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  
  int x_val = analogRead(X_pin);
  int y_val = analogRead(Y_pin);

  if(x_val < 200 || GyX < -15000){
    Serial.println("d");
  }
  else if(x_val > 900 || GyX > 15000){
    Serial.println("a");
  }
  if(y_val < 200 || GyY > 15000){
    Serial.println("s");
  }
  else if(y_val > 900 || GyY < -15000){
    Serial.println("w");
  }

  if (Serial.available() > 0){
    incomingBuzzer = Serial.read();
    for(i=0;i<100;i++)
      {
        digitalWrite(buzzer,HIGH);
        delay(2);//wait for 2ms
        digitalWrite(buzzer,LOW);
        delay(2);//wait for 2ms
      }
  }

  delay(100);
  
}
